package output

import (
	"encoding/json"
	"github.com/georg-rath/ogrt/protocol"
	"os"
)

type JsonWriter struct {
	OGWriter
	file map[string]*os.File
}

func (fw *JsonWriter) Open() {
	fw.file = make(map[string]*os.File)
}

func (fw *JsonWriter) PersistJobStart(job_start *OGRT.JobStart) {
}

func (fw *JsonWriter) PersistJobEnd(job_end *OGRT.JobEnd) {
}

func (fw *JsonWriter) PersistProcessInfo(process_info *OGRT.ProcessInfo) {
	if _, ok := fw.file[process_info.GetJobId()]; !ok {
		var err error
		if _, err := os.Stat("./jobs/"); err != nil {
			if os.IsNotExist(err) {
				os.Mkdir("./jobs", 0700)
			} else {
				panic(err)
			}
		}

		fw.file[process_info.GetJobId()], err = os.OpenFile("./jobs/"+process_info.GetJobId(), os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0600)
		if err != nil {
			panic(err)
		}
	}

	b, err := json.Marshal(process_info)
	if err != nil {
		panic(err)
	}

	if _, err := fw.file[process_info.GetJobId()].Write(b); err != nil {
		panic(err)
	}

}

func (fw *JsonWriter) Close() {
}
