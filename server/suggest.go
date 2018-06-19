package server

import (
	"strings"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/olivere/elastic"
)

func (p *suggestParam) completionSuggest() (result []string, err error) {
	result = make([]string, 0, p.Size)
	search := esClient.Search().Index(resourceIndex).Type(resourceType)

	field := "name2"
	if p.Type == "imdb" {
		field = "title2"
	} else if p.Type == "mv" {
		field = "title3"
	}
	suggester := elastic.NewCompletionSuggester("completion-suggest").
		Text(p.Text).Field(field).SkipDuplicates(true).Size(p.Size)
	search = search.Suggester(suggester)
	searchResult, err := search.Do(p.ctx)
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

func (p *suggestParam) termSuggest() (result []string, err error) {
	result = make([]string, 0, p.Size)
	search := esClient.Search().Index(resourceIndex).Type(resourceType)

	field := "title"
	if p.Type == "torrent" {
		field = "name"
	}
	suggester := elastic.NewTermSuggester("term-suggest").
		Text(p.Text).Field(field).Size(1).SuggestMode("popular")
	search = search.Suggester(suggester)
	searchResult, err := search.Do(p.ctx)
	if err != nil {
		return
	}

	suggestResult := searchResult.Suggest["term-suggest"]
	if suggestResult == nil || len(suggestResult) == 0 {
		return
	}

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

func (p *suggestParam) phaseSuggest() (result []string, err error) {
	result = make([]string, 0, p.Size)
	search := esClient.Search().Index(resourceIndex).Type(resourceType)

	field := "title"
	if p.Type == "torrent" {
		field = "name"
	}
	suggester := elastic.NewPhraseSuggester("phase-suggest").
		Text(p.Text).Field(field).Size(p.Size).GramSize(3)
	search = search.Suggester(suggester)
	searchResult, err := search.Do(p.ctx)
	if err != nil {
		return
	}

	suggestResult := searchResult.Suggest["phase-suggest"]
	if suggestResult == nil || len(suggestResult) == 0 {
		return
	}

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

func (p *suggestParam) Suggest() (result []string, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "es-suggest")
	defer seg.Close(err)

	input := strings.TrimSpace(p.Text)
	if input == "" {
		result = make([]string, 0)
		return
	}

	result, err = p.completionSuggest()
	if err != nil || len(result) > 0 {
		return
	}

	result, err = p.termSuggest()
	return
}
