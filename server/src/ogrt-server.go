package main

import (
	"encoding/binary"
	"fmt"
	"github.com/BurntSushi/toml"
	"github.com/golang/protobuf/proto"
	"io"
	"log"
	"net"
	"os"
	"os/signal"
	"output"
	"protocol"
	"strings"
	"syscall"
)

var config Configuration

type Output struct {
	Type   string
	Params string
	Writer output.OGWriter
}

type Configuration struct {
	Address          string
	Port             int
	MaxReceiveBuffer uint32
	Outputs          map[string]Output
}

func main() {
	log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds)

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

	/* instantiate all outputs */
	for name, out := range config.Outputs {
		output_gosucks := config.Outputs[name]
		switch out.Type {
		case "JsonOverTcp":
			output_gosucks.Writer = new(output.JsonOverTcpOutput)
		case "JsonElasticSearch":
			output_gosucks.Writer = new(output.JsonElasticSearchOutput)
		case "JsonFile":
			output_gosucks.Writer = new(output.JsonFileOutput)
		default:
			log.Fatal("Unkown output type: ", out.Type)
		}
		output_gosucks.Writer.Open(out.Params)
		config.Outputs[name] = output_gosucks
		defer output_gosucks.Writer.Close()

		log.Printf("Instantiated output '%s' (%s) with parameters: '%s'", name, output_gosucks.Type, output_gosucks.Params)
	}

	/* Setup signal handler for SIGKILL and SIGTERM */
	sigc := make(chan os.Signal, 1)
	signal.Notify(sigc, os.Interrupt, os.Kill, syscall.SIGTERM)
	go func(c chan os.Signal) {
		sig := <-c
		log.Printf("Caught signal %s: shutting down.\n", sig)
		listener.Close()
		for _, out := range config.Outputs {
			out.Writer.Close()
		}
		os.Exit(0)
	}(sigc)

	for {
		// Listen for an incoming connection.
		connection, err := listener.Accept()
		if err != nil {
			log.Fatal("Error accepting: ", err.Error())
			os.Exit(1)
		}

		// Handle connections in a new goroutine.
		go handleRequest(connection)
	}
}

// Handles incoming requests.
func handleRequest(conn net.Conn) {
	// Close the connection when the function exits
	defer conn.Close()

	// Read the data waiting on the connection and put it in the data buffer
	for {
		// Read header from the connection
		header := make([]byte, 8)
		n, err := conn.Read(header)
		if err == io.EOF {
			return
		} else if err != nil {
			log.Printf("Connection closed unexpectedly after receiving %d bytes with error %s.\n", n, err)
			return
		}

		// Decode type and length of packet from header
		msg_type := int32(binary.BigEndian.Uint32(header[0:4]))
		msg_length := binary.BigEndian.Uint32(header[4:8])

		if msg_length > config.MaxReceiveBuffer {
			log.Printf("OGRT_SERVER: Received message bigger than maximum receive buffer")
			return
		}
		// allocate a buffer as big as the payload and read the rest of the packet
		data := make([]byte, msg_length)
		n, err = conn.Read(data)
		if err == io.EOF {
			return
		} else if err != nil {
			log.Printf("Connection closed unexpectedly after receiving %d bytes with error %s.\n", n, err)
			return
		}

		switch msg_type {
		case OGRT.MessageType_value["JobStartMsg"]:
			msg := new(OGRT.JobStart)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				log.Printf("Error decoding ExecveMsg: %s\n", err)
				continue
			}
			log.Printf("JobStart: %s", msg.GetJobId())
			//			writer.PersistJobStart(msg)
		case OGRT.MessageType_value["JobEndMsg"]:
			msg := new(OGRT.JobEnd)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				log.Printf("Error decoding ExecveMsg: %s\n", err)
				continue
			}
			log.Printf("JobEnd: %s", msg.GetJobId())
			//			writer.PersistJobEnd(msg)
		case OGRT.MessageType_value["ProcessInfoMsg"]:
			msg := new(OGRT.ProcessInfo)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				log.Printf("Error decoding ExecveMsg: %s\n", err)
				continue
			}

			//			log.Printf("bin: name=%s, signature=%s", msg.GetBinpath(), msg.GetSignature())
			//			for _, so := range msg.GetSharedObjects() {
			//				log.Printf("\tso: name=%s, signature=%s", so.GetPath(), so.GetSignature())
			//			}
			log.Printf("Persisting JobId=%s,pid=%d,bin=%s", msg.GetJobId(), msg.GetPid(), msg.GetBinpath())
			for _, out := range config.Outputs {
				out.Writer.PersistProcessInfo(msg)
			}
			log.Printf("Persisting JobId=%s,pid=%d,bin=%s - Done.", msg.GetJobId(), msg.GetPid(), msg.GetBinpath())
		case OGRT.MessageType_value["ExecveMsg"]:
			msg := new(OGRT.Execve)

			err = proto.Unmarshal(data, msg)
			if err != nil {
				log.Printf("Error decoding ExecveMsg: %s\n", err)
				continue
			}
			args := strings.Join(msg.GetArguments(), " ")
			log.Printf("Execve: %d -> %s (%s)", msg.GetPid(), msg.GetFilename(), args)
			//writer.Persist(int64(msg.GetPid()), int64(msg.GetParentPid()), "localhost", msg.GetFilename())
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
