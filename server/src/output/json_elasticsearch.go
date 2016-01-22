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
	index string
}

func (fw *JsonElasticSearchOutput) Open(params string) {
	param_split := strings.Split(params, ":")
	if len(param_split) != 4 {
		panic("Wrong parameter specification for JsonOverTcp - did you supply it in the format \"protocol:host:port:index\"?")
	}
	fw.index = param_split[3]
	log.Println("http://"+param_split[1]+":"+param_split[2])
	client,err := elastic.NewClient(elastic.SetURL("http://rkaub0000907.kau.roche.com:9200"),elastic.SetSniff(false))
	if err != nil {
		panic(err)
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
	_,err := fw.client.Index().Index(fw.index).Type("process").BodyJson(process_info).Do()
	if err != nil {
		log.Println("Could not index JSON in ElasticSearch: ", err)
	}
}

func (fw *JsonElasticSearchOutput) Close() {
}
