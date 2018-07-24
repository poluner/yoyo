package server

import (
	"strings"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/olivere/elastic"
)

type SuggestItem struct {
	Title      string               `json:"title"`
	Type       string               `json:"type"`
}

func (p *suggestParam) Suggest() (result []string, err error) {
	ctx, seg := xray.BeginSubsegment(p.ctx, "es-suggest")
	defer seg.Close(err)

	input := strings.TrimSpace(p.Text)
	if input == "" {
		result = make([]string, 0)
		return
	}

	result = make([]string, 0, p.Size)
	search := esClient.Search().Index(suggestIndex).Type(suggestType)

	query := elastic.NewMatchQuery("title", p.Text).Operator("and").Analyzer("standard")
	suggester := elastic.NewTermSuggester("term-suggest").Text(p.Text).
		Field("title").Size(p.Size).SuggestMode("always")
	search = search.Query(query).Suggester(suggester)
	search = search.Sort("score", false).Sort("_score", false)
	search = search.Size(p.Size)
	res, err := search.Do(ctx)
	if err != nil {
		return
	}

	operationSuggest := make([]string, 0, p.Size)
	otherSuggest := make([]string, 0, p.Size)
	for _, hit := range res.Hits.Hits {
		item := SuggestItem{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		if item.Type == "operation" {
			operationSuggest = append(operationSuggest, item.Title)
		} else {
			otherSuggest = append(otherSuggest, item.Title)
		}
	}

	// 把运营配的联想词放在三四位
	otherLen := len(otherSuggest)
	if otherLen == 0 {
		result = operationSuggest
	} else if otherLen <= 2 {
		result = append(result, otherSuggest...)
		result = append(result, operationSuggest...)
	} else {
		result = append(result, otherSuggest[0:2]...)
		result = append(result, operationSuggest...)
		result = append(result, otherSuggest[2:otherLen]...)
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

func (p *suggestParam) TorrentSuggest() (result []string, err error) {
	ctx, seg := xray.BeginSubsegment(p.ctx, "torrent-suggest")
	defer seg.Close(err)

	input := strings.TrimSpace(p.Text)
	if input == "" {
		result = make([]string, 0)
		return
	}

	result = make([]string, 0, p.Size)
	search := esClient.Search().Index(resourceIndex).Type(resourceType)
	if p.Type == "" {
		completionSuggester := elastic.NewCompletionSuggester("completion-suggest").
			Text(p.Text).Field("name2").SkipDuplicates(true).Size(p.Size)
		termSuggester := elastic.NewTermSuggester("term-suggest").Text(p.Text).
			Field("name").Size(1).SuggestMode("popular")
		search = search.Suggester(completionSuggester).Suggester(termSuggester)
	} else {
		completionSuggester := elastic.NewCompletionSuggester("completion-suggest").
			Text(p.Text).Field("title2").SkipDuplicates(true).Size(p.Size)
		termSuggester := elastic.NewTermSuggester("term-suggest").Text(p.Text).
			Field("title").Size(1).SuggestMode("popular")
		search = search.Suggester(completionSuggester).Suggester(termSuggester)
	}

	searchResult, err := search.Do(ctx)
	if err != nil {
		return
	}

	suggestResult := searchResult.Suggest["completion-suggest"]
	for _, suggest := range suggestResult {
		for _, option := range suggest.Options {
			result = append(result, option.Text)
		}
	}

	if len(result) > 0 {
		return
	}

	suggestResult = searchResult.Suggest["term-suggest"]
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
