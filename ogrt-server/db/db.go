package db

import (
	"fmt"
	"github.com/jmoiron/sqlx"
	_ "github.com/mattn/go-sqlite3"
	"log"
	"os"
)

type PidInfo struct {
	Pid            int64  `db:"pid"`
	PidParent      int64  `db:"pid_parent"`
	Hostname       string `db:"hostname"`
	ExecutablePath string `db:"exec_path"`
}

type OGWriter interface {
	Persist(pid int64, parentPid int64, msg_type string, exec string)
	Connect()
}

type DBWriter struct {
	OGWriter
	db *sqlx.DB
}

func (dbw *DBWriter) Connect() {
	var err error
	dbw.db, err = sqlx.Open("sqlite3", "./ogrt.db")
	if err != nil {
		log.Fatal("Error connecting to database:", err)
	}
	var schema = `
		create table pids (
			jobid integer,
			pid integer,
			pid_parent integer,
			hostname text,
			uuid text,
			exec_path text
		);`
	dbw.db.MustExec(schema)
}

func (dbw *DBWriter) Persist(pid int64, parentPid int64, msg_type string, exec string) {
	log.Printf("Persist %d \n", pid)
	_, err := dbw.db.NamedExec("INSERT INTO pids (pid, pid_parent, hostname, exec_path) VALUES (:pid, :pid_parent, :hostname, :exec_path)", &PidInfo{pid, parentPid, "testhost", exec})
	if err != nil {
		log.Println("Could not persist:", err)
	}

}

type FileWriter struct {
	OGWriter
	file *os.File
}

func (fw *FileWriter) Connect() {
	var err error
	fw.file, err = os.OpenFile("ogrt-server.log", os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0600)
	if err != nil {
		panic(err)
	}
}

func (fw *FileWriter) Persist(pid int64, parentPid int64, msg_type string, exec string) {
	out := fmt.Sprintf("Pid %d with parent %d spawned process '%s'.\n", pid, parentPid, exec)
	if _, err := fw.file.WriteString(out); err != nil {
		panic(err)
	}
}
