package output

import (
	"github.com/jmoiron/sqlx"
	_ "github.com/mattn/go-sqlite3"
	"log"
)

type DBWriter struct {
	OGWriter
	db *sqlx.DB
}

func (dbw *DBWriter) Open() {
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

func (dbw *DBWriter) Close() {
	dbw.db.Close()
}
