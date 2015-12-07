package output

import (
	"encoding/json"
	"github.com/georg-rath/ogrt/protocol"
	"log"
	"net"
	"strings"
)

type JsonOverTcpOutput struct {
	OGWriter
	encoder *json.Encoder
}

func (fw *JsonOverTcpOutput) Open(params string) {
	param_split := strings.Split(params, ":")
	if len(param_split) != 3 {
		panic("Wrong parameter specification for JsonOverTcp - did you supply it in the format \"protocol:host:port\"?")
	}
	connex, err := net.Dial(param_split[0], strings.Join(param_split[1:], ":"))
	if err != nil {
		log.Fatal(err)
	}
	fw.encoder = json.NewEncoder(connex)
}

func (fw *JsonOverTcpOutput) PersistJobStart(job_start *OGRT.JobStart) {
}

func (fw *JsonOverTcpOutput) PersistJobEnd(job_end *OGRT.JobEnd) {
}

func (fw *JsonOverTcpOutput) PersistProcessInfo(process_info *OGRT.ProcessInfo) {
	err := fw.encoder.Encode(process_info)
	if err != nil {
		log.Println("Could not send JSON over TCP: ", err)
	}
}

func (fw *JsonOverTcpOutput) Close() {
}
