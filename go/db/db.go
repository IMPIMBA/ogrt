package db

import (
	"fmt"
	"github.com/jmoiron/sqlx"
	_ "github.com/mattn/go-sqlite3"
)

type PidInfo struct {
	JobId          int64  `db:"jobid"`
	Pid            int64  `db:"pid"`
	PidParent      int64  `db:"pid_parent"`
	Hostname       string `db:"hostname"`
	UUID           string `db:"uuid"`
	ExecutablePath string `db:"exec_path"`
}

var db *sqlx.DB

func Connect() {
	var err error
	db, err = sqlx.Open("sqlite3", "./ogrt.db")
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
	db.MustExec(schema)
	fmt.Println(db)
}

func Persist(pid int64) {
	fmt.Printf("Persist %d \n", pid)
	_, err := db.NamedExec("INSERT INTO pids (jobid, pid, pid_parent, hostname, uuid, exec_path) VALUES (:jobid, :pid, :pid_parent, :hostname, :uuid, :exec_path)", &PidInfo{1, pid, pid, "testhost", "uuid", "path"})
	if err != nil {
		fmt.Println(err)
	}

}
