package output

import (
	"encoding/json"
	"github.com/georg-rath/ogrt/protocol"
	"io/ioutil"
	"log"
	"os"
	"path"
	"sync"
)

type JsonFileOutput struct {
	OGWriter
	path string
	mu   sync.Mutex
}

func (fw *JsonFileOutput) Open(params string) {
	if len(params) == 0 {
		log.Panic("Please specify an output path for the JsonFile output")
	}
	fw.path = params
	if _, err := os.Stat(fw.path); err != nil {
		if os.IsNotExist(err) {
			os.Mkdir(fw.path, 0700)
		} else {
			log.Panic(err)
		}
	}
}

func (fw *JsonFileOutput) PersistJobStart(job_start *OGRT.JobStart) {
}

func (fw *JsonFileOutput) PersistJobEnd(job_end *OGRT.JobEnd) {
}

func (fw *JsonFileOutput) PersistProcessInfo(process_info *OGRT.ProcessInfo) {
	filepath := path.Join(fw.path, process_info.GetJobId())
	var job_info OGRT.JobInfo
	fw.mu.Lock()
	json_bytes, err := ioutil.ReadFile(filepath)
	if err == nil {
		err = json.Unmarshal(json_bytes, &job_info)
		if err != nil {
			log.Panic(err)
		}
	} else {
		jid := process_info.GetJobId()
		job_info.JobId = &jid
	}
	job_info.Processes = append(job_info.Processes, process_info)

	b, err := json.Marshal(job_info)
	if err != nil {
		log.Panic(err)
	}

	file, err := os.Create(filepath)
	if _, err := file.Write(b); err != nil {
		log.Panic(err)
	}
	file.Close()
	fw.mu.Unlock()
}

func (fw *JsonFileOutput) Close() {
}
