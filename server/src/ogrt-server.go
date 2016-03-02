package main

import (
	"encoding/binary"
	"fmt"
	"github.com/BurntSushi/toml"
	"github.com/golang/protobuf/proto"
	"github.com/rcrowley/go-metrics"
	"github.com/rcrowley/go-metrics/exp"
	"io"
	"log"
	"net"
	"net/http"
	"os"
	"os/signal"
	"output"
	"protocol"
	"syscall"
	"time"
)

var VERSION string

var config Configuration

type Output struct {
	Type    string
	Params  string
	Workers int
	Writer  output.OGWriter
}

type Configuration struct {
	Address          string
	Port             int
	MaxReceiveBuffer uint32
	DebugEndpoint    bool
	Outputs          map[string]Output
}

var outputs map[string][]Output
var output_channels map[string]chan interface{}

func main() {
	log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds)

	log.Printf("ogrt-server %s", VERSION)

	if _, err := toml.DecodeFile("ogrt.conf", &config); err != nil {
		log.Fatal(err)
	}

	/* expose metrics as HTTP endpoint */
	if config.DebugEndpoint == true {
		exp.Exp(metrics.DefaultRegistry)
		go http.ListenAndServe(":8080", nil)
		log.Printf("Instantiated DebugEndpoint at Port 8080")
	}

	// Listen for incoming connections.
	listen_string := fmt.Sprintf("%s:%d", config.Address, config.Port)
	listener, err := net.Listen("tcp", listen_string)
	if err != nil {
		log.Fatal("Error listening:", err.Error())
	}
	// Close the listener when the application closes.
	defer listener.Close()

	outputs = make(map[string][]Output)
	output_channels = make(map[string]chan interface{})

	/* instantiate all outputs */
	for name, out := range config.Outputs {
		output_channels[name] = make(chan interface{})
		for i := 0; i < config.Outputs[name].Workers; i++ {
			var output_gosucks Output
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

			outputs[name] = append(outputs[name], output_gosucks)
			go writeToOutput(name, i, output_channels[name])
			defer output_gosucks.Writer.Close()
		}

		metrics.Register("output_"+name, metrics.NewTimer())
		log.Printf("Instantiated output '%s' of type '%s' with parameters: '%s'", name, config.Outputs[name].Type, config.Outputs[name].Params)
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

	/* register timer for accept() */
	accept_timer := metrics.NewTimer()
	metrics.Register("accept", accept_timer)

	/* output metrics every five seconds */
	go metrics.LogScaled(metrics.DefaultRegistry, 5*time.Second, time.Millisecond, log.New(os.Stderr, "metrics: ", log.Lmicroseconds))

	for {
		// Listen for an incoming connection.
		accept_timer.Time(func() {
			connection, err := listener.Accept()
			if err != nil {
				log.Fatal("Error accepting: ", err.Error())
				os.Exit(1)
			}
			go handleRequest(connection)
		})
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

		go func() {
			switch msg_type {
			case OGRT.MessageType_value["ProcessInfoMsg"]:
				msg := new(OGRT.ProcessInfo)

				err = proto.Unmarshal(data, msg)
				if err != nil {
					log.Printf("Error decoding ExecveMsg: %s\n", err)
					return
				}

				for _, c := range output_channels {
					c <- msg
				}
			}
		}()
	}
}

func writeToOutput(output string, id int, messages chan interface{}) {
	out := outputs[output][id]
	for message := range messages {
		switch message := message.(type) {
		default:
			log.Printf("unexpected type %T", message)
		case *OGRT.ProcessInfo:
			metric := metrics.Get("output_" + output).(metrics.Timer)
			log.Printf("%d: Persisting JobId=%s,pid=%d,bin=%s", id, message.GetJobId(), message.GetPid(), message.GetBinpath())
			metric.Time(func() { out.Writer.PersistProcessInfo(message) })
			log.Printf("%d: Persisting JobId=%s,pid=%d,bin=%s - Done", id, message.GetJobId(), message.GetPid(), message.GetBinpath())
		}
	}
	log.Printf("done")
}
