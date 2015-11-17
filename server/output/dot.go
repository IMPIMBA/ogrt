package output

import (
	"fmt"
	"os"
)

type GraphWriter struct {
	OGWriter
	file *os.File
}

func (fw *GraphWriter) Open() {
	var err error
	fw.file, err = os.OpenFile("ogrt-server.log", os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0600)
	if err != nil {
		panic(err)
	}

	if _, err := fw.file.WriteString("digraph ogrt {\n\tnode[shape=record]\n"); err != nil {
		panic(err)
	}
}

func (fw *GraphWriter) Persist(pid int64, parentPid int64, msg_type string, exec string) {
	out := fmt.Sprintf("\t%d -> %d [label=\"exec\"];\n\t%d[label=\"{%d|%s}\"];\n", parentPid, pid, pid, pid, exec)
	if _, err := fw.file.WriteString(out); err != nil {
		panic(err)
	}
}

func (fw *GraphWriter) Close() {
	if _, err := fw.file.WriteString("}"); err != nil {
		panic(err)
	}

	if err := fw.file.Close(); err != nil {
		panic(err)
	}
}
