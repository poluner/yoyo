package server

import (
	"encoding/json"
	"github.com/olivere/elastic"
	"golang.org/x/net/context"
	"strings"
	"time"
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

type FileItem struct {
	Path   []string `json:"path"`
	Length int64    `json:"length"`
}

type EsTorrent struct {
	Name        string     `json:"name"`
	Download    int        `json:"hot"`
	Length      int64      `json:"length"`
	CollectedAt time.Time  `json:"collected_at"`
	Files       []FileItem `json:"files,omitempty"`
}

type Torrent struct {
	Infohash    string              `json:"infohash"`
	Name        string              `json:"name"`
	Length      int64               `json:"length"`
	Download    int                 `json:"download"`
	CollectedAt JsonTime            `json:"collected_at"`
	Files       []FileItem          `json:"files,omitempty"`
	Highlight   map[string][]string `json:"highlight,omitempty"`
}

// NewCompletionSuggester 添加 SkipDuplicate 字段
func completionSuggest(text string, size int) (result []string, err error) {
	result = make([]string, 0, size)
	search := esClient.Search().Index(esIndex).Type(esType)
	suggester := elastic.NewCompletionSuggester("completion-suggest").
		Text(text).Field("name2").SkipDuplicates(true).Size(size)
	search = search.Suggester(suggester)
	searchResult, err := search.Do(context.Background())
	if err != nil {
		return
	}

	suggestResult := searchResult.Suggest["completion-suggest"]
	for _, suggest := range suggestResult {
		for _, option := range suggest.Options {
			result = append(result, option.Text)
		}
	}
	return
}

func termSuggest(text string) (result []string, err error) {
	result = make([]string, 0, 1)
	search := esClient.Search().Index(esIndex).Type(esType)
	suggester := elastic.NewTermSuggester("term-suggest").
		Text(text).Field("name").Size(1).SuggestMode("popular")
	search = search.Suggester(suggester)
	searchResult, err := search.Do(context.Background())
	if err != nil {
		return
	}

	suggestResult := searchResult.Suggest["term-suggest"]
	if suggestResult == nil || len(suggestResult) == 0 {
		return
	}

	for _, suggest := range suggestResult {
		if suggest.Options == nil || len(suggest.Options) == 0 {
			result = append(result, suggest.Text)
		} else {
			result = append(result, suggest.Options[0].Text)
		}
	}

	return
}

func EsSuggest(text string, size int) (result []string, err error) {
	input := strings.TrimSpace(text)
	if input == "" {
		result = make([]string, 0)
		return
	}

	result, err = completionSuggest(input, size)
	if err != nil || len(result) > 0 {
		return
	}

	result, err = termSuggest(input)
	return
}

func EsSearch(text string, offset int, limit int) (total int64, result []Torrent, err error) {
	result = make([]Torrent, 0, limit)
	input := strings.TrimSpace(text)
	search := esClient.Search().Index(esIndex).Type(esType)
	if input == "" {
		query := elastic.NewMatchAllQuery()
		search = search.Query(query)
	} else {
		query := elastic.NewBoolQuery()
		nameQuery := elastic.NewMatchQuery("name", input)
		pathQuery := elastic.NewMatchQuery("files.path", input)
		query = query.Should(nameQuery, pathQuery)
		scoreFilter := elastic.NewRangeQuery("_score").Gte(1.0)
		query = query.Filter(scoreFilter)
		highlight := elastic.NewHighlight().Field("name")
		search = search.Query(query).Highlight(highlight)
		search = search.Sort("_score", false)
	}

	search = search.From(offset).Size(limit)
	res, err := search.Do(context.Background())
	if err != nil {
		return
	}

	total = res.TotalHits()
	for _, hit := range res.Hits.Hits {
		item := EsTorrent{}
		err = json.Unmarshal(*hit.Source, &item)
		if err != nil {
			continue
		}

		result = append(result, Torrent{
			Infohash:    hit.Id,
			Name:        item.Name,
			Length:      item.Length,
			Download:    item.Download,
			Files:       item.Files,
			CollectedAt: JsonTime{item.CollectedAt},
			Highlight:   hit.Highlight,
		})
	}
	return
}

func EsUpdateHot(infohash string) (err error) {
	ctx := context.Background()
	get, err := esClient.Get().Index(esIndex).Type(esType).Id(infohash).Do(ctx)
	if err != nil {
		return
	}

	item := EsTorrent{}
	err = json.Unmarshal(*get.Source, &item)
	if err != nil {
		return
	}

	script := elastic.NewScript("ctx._source.hot=params.hot").Param("hot", item.Download+1)
	_, err = esClient.Update().Index(esIndex).Type(esType).Id(infohash).Script(script).Do(ctx)
	return
}
