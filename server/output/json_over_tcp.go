package output

import (
	"encoding/json"
	"github.com/georg-rath/ogrt/protocol"
	"net"
)

type JsonOverTcpOutput struct {
	OGWriter
	encoder *json.Encoder
}

func (fw *JsonOverTcpOutput) Open() {
	connex, err := net.Dial("tcp", "spl-impimba-1:18255")
	if err != nil {
		panic(err)
	}
	fw.encoder = json.NewEncoder(connex)
}

func (fw *JsonOverTcpOutput) PersistJobStart(job_start *OGRT.JobStart) {
}

func (fw *JsonOverTcpOutput) PersistJobEnd(job_end *OGRT.JobEnd) {
}

func (fw *JsonOverTcpOutput) PersistProcessInfo(process_info *OGRT.ProcessInfo) {
	fw.encoder.Encode(process_info)
}

func (fw *JsonOverTcpOutput) Close() {
}
