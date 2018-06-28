package server

import (
	"strings"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/olivere/elastic"
)

type SuggestItem struct {
	Title      string               `json:"title"`
	Type       string               `json:"type"`
	Score      int                  `json:"score"`
	Id         string               `json:"id"`
}

func (p *suggestParam) Suggest() (result []string, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "es-suggest")
	defer seg.Close(err)

	input := strings.TrimSpace(p.Text)
	if input == "" {
		result = make([]string, 0)
		return
	}

	result = make([]string, 0, p.Size)
	search := esClient.Search().Index(suggestIndex).Type(suggestType)

	query := elastic.NewMatchQuery("title", p.Text).Operator("and")
	suggester := elastic.NewTermSuggester("term-suggest").Text(p.Text).
		Field("title").Size(p.Size).SuggestMode("always")
	search = search.Query(query).Suggester(suggester)
	search = search.Sort("_score", false).Sort("score", false)
	search = search.Size(p.Size)
	res, err := search.Do(p.ctx)
	if err != nil {
		return
	}

	for _, hit := range res.Hits.Hits {
		item := SuggestItem{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		result = append(result, item.Title)
	}

	if len(result) > 0 {
		return
	}

	suggestResult := res.Suggest["term-suggest"]
	for _, suggest := range suggestResult {
		if len(result) >= p.Size {
			break
		}


		for _, item := range suggest.Options {
			result = append(result, item.Text)
		}
	}
	return
}
