package output

import (
	"encoding/json"
	"github.com/georg-rath/ogrt/protocol"
	"io/ioutil"
	"os"
)

type JsonWriter struct {
	OGWriter
	file map[string]*os.File
}

func (fw *JsonWriter) Open() {
	fw.file = make(map[string]*os.File)

	if _, err := os.Stat("./jobs/"); err != nil {
		if os.IsNotExist(err) {
			os.Mkdir("./jobs", 0700)
		} else {
			panic(err)
		}
	}
}

func (fw *JsonWriter) PersistJobStart(job_start *OGRT.JobStart) {
}

func (fw *JsonWriter) PersistJobEnd(job_end *OGRT.JobEnd) {
}

func (fw *JsonWriter) PersistProcessInfo(process_info *OGRT.ProcessInfo) {
	filepath := "./jobs/" + process_info.GetJobId()
	var job_info OGRT.JobInfo
	json_bytes, err := ioutil.ReadFile(filepath)
	if err == nil {
		err = json.Unmarshal(json_bytes, &job_info)
		if err != nil {
			panic(err)
		}
	} else {
		jid := process_info.GetJobId()
		job_info.JobId = &jid
	}
	job_info.Processes = append(job_info.Processes, process_info)

	b, err := json.Marshal(job_info)
	if err != nil {
		panic(err)
	}

	file, err := os.Create(filepath)
	if _, err := file.Write(b); err != nil {
		panic(err)
	}
	file.Close()
}

func (fw *JsonWriter) Close() {
}
