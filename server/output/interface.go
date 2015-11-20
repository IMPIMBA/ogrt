package output

import (
	"github.com/georg-rath/ogrt/protocol"
)

type OGWriter interface {
	PersistJobStart(msg *OGRT.JobStart)
	PersistJobEnd(msg *OGRT.JobEnd)
	PersistProcessInfo(msg *OGRT.ProcessInfo)
	Open()
	Close()
}
