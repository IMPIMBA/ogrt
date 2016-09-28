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
	PrintMetrics     uint32
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
		log.Printf("Instantiated DebugEndpoint at Port 8080 (http://0.0.0.0:8080/debug/metrics)")
	}

	// Listen for incoming connections.
	listen_string := fmt.Sprintf("%s:%d", config.Address, config.Port)
	ServerAddr, err := net.ResolveUDPAddr("udp", listen_string)
	if err != nil {
		log.Fatal("Error resolving UDP address:", err.Error())
	}
	listener, err := net.ListenUDP("udp", ServerAddr)
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
			var output_ Output
			switch out.Type {
			case "JsonOverTcp":
				output_.Writer = new(output.JsonOverTcpOutput)
			case "JsonElasticSearch":
				output_.Writer = new(output.JsonElasticSearchOutput)
			case "JsonFile":
				output_.Writer = new(output.JsonFileOutput)
			default:
				log.Fatal("Unkown output type: ", out.Type)
			}
			output_.Writer.Open(out.Params)

			outputs[name] = append(outputs[name], output_)
			go writeToOutput(name, i, output_channels[name])
			defer output_.Writer.Close()
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

	/* register timer for receive() */
	receive_timer := metrics.NewTimer()
	metrics.Register("receive", receive_timer)

	/* output metrics on stderr */
	if config.PrintMetrics > 0 {
		log.Printf("printing metrics every %d seconds", config.PrintMetrics)
		go metrics.LogScaled(metrics.DefaultRegistry, time.Duration(config.PrintMetrics)*time.Second, time.Millisecond, log.New(os.Stderr, "metrics: ", log.Lmicroseconds))
	}

	packet_buffer := make([]byte, config.MaxReceiveBuffer)
	// Read the data waiting on the connection and put it in the data buffer
	for {
		// Read header from the connection
		n, addr, err := listener.ReadFromUDP(packet_buffer)
		log.Printf("read %d bytes from %s", n, addr)
		if err == io.EOF {
			return
		} else if err != nil {
			log.Printf("error while receiving from %s, bytes: %d, error: %s", addr, n, err)
			return
		}

		receive_timer.Time(func() {
			// Decode type and length of packet from header
			msg_type := int32(binary.BigEndian.Uint32(packet_buffer[0:4]))
			msg_length := binary.BigEndian.Uint32(packet_buffer[4:8])

			log.Printf("type: %d, length: %d", msg_type, msg_length)

			// allocate a buffer as big as the payload and read the rest of the packet
			data := packet_buffer[8 : msg_length+8]

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
		})
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
