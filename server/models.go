package server

import (
	"github.com/olivere/elastic"
)

var (
	esClient *elastic.Client
)

func init() {
	var err error

	esClient, err = elastic.NewClient(elastic.SetURL(EsUrls...))
	if err != nil {
		panic(err)
	}
}

func esSuggest(text string, size int) (result []string, err error) {
	result = make([]string, 0, size)
	return
}
