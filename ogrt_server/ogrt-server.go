package main

import (
	"encoding/binary"
	"fmt"
	"github.com/BurntSushi/toml"
	"github.com/georg-rath/ogrt/db"
	"github.com/georg-rath/ogrt/messages"
	"github.com/golang/protobuf/proto"
	"io"
	"net"
	"os"
	"os/signal"
	"syscall"
)

var writer db.OGWriter

type Configuration struct {
	Address string
	Port    int
}

func main() {
	var config Configuration
	if _, err := toml.DecodeFile("ogrt.conf", &config); err != nil {
		fmt.Println(err)
		return
	}

	// Listen for incoming connections.
	listen_string := fmt.Sprintf("%s:%d", config.Address, config.Port)
	l, err := net.Listen("tcp", listen_string)
	if err != nil {
		fmt.Println("Error listening:", err.Error())
		os.Exit(1)
	}
	// Close the listener when the application closes.
	defer l.Close()

	writer = new(db.DBWriter)
	writer.Connect()

	sigc := make(chan os.Signal, 1)
	signal.Notify(sigc, os.Interrupt, os.Kill, syscall.SIGTERM)
	go func(c chan os.Signal) {
		// Wait for a SIGINT or SIGKILL:
		sig := <-c
		fmt.Printf("Caught signal %s: shutting down.\n", sig)
		// Stop listening (and unlink the socket if unix type):
		l.Close()
		// And we're done:
		os.Exit(0)
	}(sigc)

	for {
		// Listen for an incoming connection.
		conn, err := l.Accept()
		if err != nil {
			fmt.Println("Error accepting: ", err.Error())
			os.Exit(1)
		}

		fmt.Println("Opened connection")
		// Handle connections in a new goroutine.
		go handleRequest(conn)
	}
}

// Handles incoming requests.
func handleRequest(conn net.Conn) {
	fmt.Println("Handling packet...")
	// Close the connection when the function exits
	defer conn.Close()
	// Read the data waiting on the connection and put it in the data buffer
	for {
		// Read header from the connection
		header := make([]byte, 8)
		n, err := conn.Read(header)
		if err == io.EOF {
			fmt.Printf("Connection closed by remote end.\n")
			return
		} else if err != nil {
			fmt.Printf("Connection closed unexpectedly after receiving %d bytes.\n", n)
			fmt.Println(err)
			return
		}

		// Decode type and length of packet from header
		msg_type := int32(binary.BigEndian.Uint32(header[0:4]))
		msg_length := binary.BigEndian.Uint32(header[4:8])
		fmt.Printf("type %d length %d \n", msg_type, msg_length)

		// allocate a buffer as big as the payload and read the rest of the packet
		data := make([]byte, msg_length)
		n, err = conn.Read(data)
		if err == io.EOF {
			fmt.Printf("Connection closed by remote end.\n")
			return
		} else if err != nil {
			fmt.Printf("Connection closed unexpectedly after receiving %d bytes.\n", n)
			fmt.Println(err)
			return
		}

		fmt.Printf("Decoding Protobuf message with size %d (advertised %d)\n", n, msg_length)

		switch msg_type {
		case OGRT.MessageType_value["ExecveMsg"]:
			msg := new(OGRT.Execve)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				fmt.Println("error decoding")
			}
			fmt.Printf("Execve: %d -> %s \n", msg.GetPid(), msg.GetFilename())
			for _, arg := range msg.GetArguments() {
				fmt.Println(arg)
			}
			writer.Persist(int64(msg.GetPid()))
		case OGRT.MessageType_value["ForkMsg"]:
			msg := new(OGRT.Fork)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				fmt.Println("error decoding")
			}
			fmt.Printf("Fork: %d -> %d \n", msg.GetParentPid(), msg.GetChildPid())
			writer.Persist(int64(msg.GetParentPid()))
		}
	}
}
