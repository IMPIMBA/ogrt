package db

import (
	"fmt"
	"github.com/jmoiron/sqlx"
	_ "github.com/mattn/go-sqlite3"
)

type OGWriter interface {
	Persist(int64)
	Connect()
}

type DBWriter struct {
	OGWriter
	db *sqlx.DB
}

type PidInfo struct {
	Pid            int64  `db:"pid"`
	PidParent      int64  `db:"pid_parent"`
	Hostname       string `db:"hostname"`
	ExecutablePath string `db:"exec_path"`
}

func (dbw *DBWriter) Connect() {
	var err error
	dbw.db, err = sqlx.Open("sqlite3", "./ogrt.db")
	if err != nil {
		fmt.Println("nope")
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

func (dbw *DBWriter) Persist(pid int64) {
	fmt.Printf("Persist %d \n", pid)
	_, err := dbw.db.NamedExec("INSERT INTO pids (jobid, pid, pid_parent, hostname, uuid, exec_path) VALUES (:jobid, :pid, :pid_parent, :hostname, :uuid, :exec_path)", &PidInfo{1, pid, pid, "testhost", "uuid", "path"})
	if err != nil {
		fmt.Println(err)
	}

}
