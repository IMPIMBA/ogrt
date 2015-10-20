package output

import ()

type PidInfo struct {
	Pid            int64  `db:"pid"`
	PidParent      int64  `db:"pid_parent"`
	Hostname       string `db:"hostname"`
	ExecutablePath string `db:"exec_path"`
}

type OGWriter interface {
	Persist(pid int64, parentPid int64, msg_type string, exec string)
	Open()
	Close()
}
