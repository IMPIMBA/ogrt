package main

import (
	"code.google.com/p/goprotobuf/proto"
	"fmt"
	"github.com/georg-rath/ogrt/messages"
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
	data := make([]byte, 16384)
	//Read the data waiting on the connection and put it in the data buffer
	for {
		n, err := conn.Read(data)
		if err != nil {
			fmt.Printf("Received %d bytes\n", n)
			return
		}
		fmt.Println("Decoding Protobuf message")

		protodata := new(ogrt.Execve)

		err = proto.Unmarshal(data[0:n], protodata)
		if err != nil {
			fmt.Println("error decoding")
		}

		fmt.Println(protodata.GetPid())
		fmt.Println(protodata.GetFilename())
		for _, element := range protodata.GetEnvironmentVariables() {
			fmt.Println(element)
		}

		for _, element := range protodata.GetArguments() {
			fmt.Println(element)
		}
	}
}
