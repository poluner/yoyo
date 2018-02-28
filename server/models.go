package server

import (
	"encoding/json"
	"fmt"
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

func EsSuggest(text string, size int) (result []string, err error) {
	result = make([]string, 0, size)
	s := elastic.NewFuzzyCompletionSuggester("torrent-suggest").
		Text(text).Field("name2").Fuzziness(2).Size(size)
	src, err := s.Source(true)
	data, err := json.Marshal(src)

	fmt.Println(string(data))
	return
}
