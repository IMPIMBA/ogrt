package output

import (
	"log"
	"protocol"
	"strings"

	"gopkg.in/olivere/elastic.v3"
)

type JsonElasticSearchOutput struct {
	OGWriter
	client *elastic.Client
	index  string
}

func (fw *JsonElasticSearchOutput) Open(params string) {
	param_split := strings.Split(params, ":")
	if len(param_split) != 4 {
		panic("Wrong parameter specification for JsonOverElasticSearch - did you supply it in the format \"protocol:host:port:index\"?")
	}
	protocol := param_split[0]
	host := param_split[1]
	port := param_split[2]
	index := param_split[3]

	fw.index = index

	client, err := elastic.NewClient(elastic.SetURL(protocol+"://"+host+":"+port), elastic.SetSniff(false))
	if err != nil {
		log.Fatal(err)
	}
	fw.client = client
}

func (fw *JsonElasticSearchOutput) PersistJobStart(job_start *OGRT.JobStart) {
}

func (fw *JsonElasticSearchOutput) PersistJobEnd(job_end *OGRT.JobEnd) {
}

func (fw *JsonElasticSearchOutput) PersistProcessInfo(process_info *OGRT.ProcessInfo) {
	// set time to milliseconds
	*process_info.Time = *process_info.Time * int64(1000)
	_, err := fw.client.Index().Index(fw.index).Type("process").BodyJson(process_info).Do()
	if err != nil {
		log.Println("Could not index JSON in ElasticSearch: ", err)
	}
}

func (fw *JsonElasticSearchOutput) Close() {
}
