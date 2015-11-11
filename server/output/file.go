package output

import (
	"fmt"
	"os"
)

type FileWriter struct {
	OGWriter
	file *os.File
}

func (fw *FileWriter) Open() {
	var err error
	fw.file, err = os.OpenFile("ogrt-server.log", os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0600)
	if err != nil {
		panic(err)
	}
}

func (fw *FileWriter) Persist(pid int64, parentPid int64, msg_type string, exec string) {
	out := fmt.Sprintf("%d -exec-> %d [%s]\n", parentPid, pid, exec)
	if _, err := fw.file.WriteString(out); err != nil {
		panic(err)
	}
}

func (fw *FileWriter) Close() {
	if err := fw.file.Close(); err != nil {
		panic(err)
	}
}
