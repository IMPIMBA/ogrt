package main

import (
	"encoding/binary"
	"fmt"
	"github.com/georg-rath/ogrt/db"
	"github.com/georg-rath/ogrt/messages"
	"github.com/golang/protobuf/proto"
	"io"
	"net"
	"os"
	"os/signal"
	"syscall"
)

func main() {
	// Listen for incoming connections.
	l, err := net.Listen("unix", "/tmp/ogrt.sock")
	if err != nil {
		fmt.Println("Error listening:", err.Error())
		os.Exit(1)
	}
	// Close the listener when the application closes.
	defer l.Close()

	db.Connect()
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
	//Close the connection when the function exits
	defer conn.Close()
	//Create a data buffer of type byte slice with capacity of 16k
	//Read the data waiting on the connection and put it in the data buffer
	for {
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

		msg_type := int32(binary.BigEndian.Uint32(header[0:4]))
		msg_length := binary.BigEndian.Uint32(header[4:8])
		fmt.Printf("type %d length %d \n", msg_type, msg_length)

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
			db.Persist(int64(msg.GetPid()))
		case OGRT.MessageType_value["ForkMsg"]:
			msg := new(OGRT.Fork)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				fmt.Println("error decoding")
			}
			fmt.Printf("Fork: %d -> %d \n", msg.GetParentPid(), msg.GetChildPid())
			db.Persist(int64(msg.GetParentPid()))
		}
	}
}
