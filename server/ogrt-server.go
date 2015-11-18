package main

import (
	"encoding/binary"
	"fmt"
	"github.com/BurntSushi/toml"
	"github.com/georg-rath/ogrt/output"
	"github.com/georg-rath/ogrt/protocol"
	"github.com/golang/protobuf/proto"
	"io"
	"log"
	"net"
	"os"
	"os/signal"
	"strings"
	"syscall"
)

var writer output.OGWriter
var config Configuration

type Configuration struct {
	Address          string
	Port             int
	MaxReceiveBuffer uint32
}

func main() {
	if _, err := toml.DecodeFile("ogrt.conf", &config); err != nil {
		log.Fatal(err)
	}

	// Listen for incoming connections.
	listen_string := fmt.Sprintf("%s:%d", config.Address, config.Port)
	listener, err := net.Listen("tcp", listen_string)
	if err != nil {
		log.Fatal("Error listening:", err.Error())
	}
	// Close the listener when the application closes.
	defer listener.Close()

	writer = new(output.FileWriter)
	writer.Open()
	defer writer.Close()

	/* Setup signal handler for SIGKILL and SIGTERM */
	sigc := make(chan os.Signal, 1)
	signal.Notify(sigc, os.Interrupt, os.Kill, syscall.SIGTERM)
	go func(c chan os.Signal) {
		sig := <-c
		log.Printf("Caught signal %s: shutting down.\n", sig)
		listener.Close()
		writer.Close()
		os.Exit(0)
	}(sigc)

	for {
		// Listen for an incoming connection.
		connection, err := listener.Accept()
		if err != nil {
			log.Fatal("Error accepting: ", err.Error())
			os.Exit(1)
		}

		log.Println("Opened connection")
		// Handle connections in a new goroutine.
		go handleRequest(connection)
	}
}

// Handles incoming requests.
func handleRequest(conn net.Conn) {
	log.Println("Handling packet...")
	// Close the connection when the function exits
	defer conn.Close()
	// Read the data waiting on the connection and put it in the data buffer
	for {
		// Read header from the connection
		header := make([]byte, 8)
		n, err := conn.Read(header)
		if err == io.EOF {
			log.Println("Connection closed by remote end.")
			return
		} else if err != nil {
			log.Printf("Connection closed unexpectedly after receiving %d bytes with error %s.\n", n, err)
			return
		}

		// Decode type and length of packet from header
		msg_type := int32(binary.BigEndian.Uint32(header[0:4]))
		msg_length := binary.BigEndian.Uint32(header[4:8])
		log.Printf("type %d length %d \n", msg_type, msg_length)

		if msg_length > config.MaxReceiveBuffer {
			log.Printf("OGRT_SERVER: Received message bigger than maximum receive buffer")
			return
		}
		// allocate a buffer as big as the payload and read the rest of the packet
		data := make([]byte, msg_length)
		n, err = conn.Read(data)
		if err == io.EOF {
			log.Println("Connection closed by remote end.\n")
			return
		} else if err != nil {
			log.Printf("Connection closed unexpectedly after receiving %d bytes with error %s.\n", n, err)
			return
		}

		log.Printf("Decoding Protobuf message with size %d (advertised %d)\n", n, msg_length)

		switch msg_type {
		case OGRT.MessageType_value["ProcessInfoMsg"]:
			msg := new(OGRT.ProcessInfo)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				log.Printf("Error decoding ExecveMsg: %s\n", err)
				continue
			}

			log.Printf("bin: name=%s, signature=%s", msg.GetBinpath(), msg.GetSignature())
			for _, so := range msg.GetSharedObject() {
				log.Printf("\tso: name=%s, signature=%s", so.GetPath(), so.GetSignature())
			}
		case OGRT.MessageType_value["ExecveMsg"]:
			msg := new(OGRT.Execve)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				log.Printf("Error decoding ExecveMsg: %s\n", err)
				continue
			}
			args := strings.Join(msg.GetArguments(), " ")
			log.Printf("Execve: %d -> %s (%s)", msg.GetPid(), msg.GetFilename(), args)
			writer.Persist(int64(msg.GetPid()), int64(msg.GetParentPid()), "localhost", msg.GetFilename())
		case OGRT.MessageType_value["ForkMsg"]:
			msg := new(OGRT.Fork)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				log.Printf("Error decoding ExecveMsg: %s\n", err)
				continue
			}
			log.Printf("Fork: %d -> %d \n", msg.GetParentPid(), msg.GetChildPid())
			//	writer.Persist(int64(msg.GetParentPid()))
		}

	}
}
