package server

import (
	"context"
	"encoding/json"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/olivere/elastic"
	"strings"
	"time"
	log "github.com/alecthomas/log4go"
)

const (
	maxResultWindow = 8000
)

var (
	esClient *elastic.Client
)

type FileItem struct {
	Path   []string `json:"path"`
	Length int      `json:"length"`
}

type EsTorrent struct {
	Name        string     `json:"name"`
	Name2       string     `json:"name2"`
	Type        string     `json:"type,omitempty"`
	Download    int        `json:"hot"`
	Length      int        `json:"length"`
	CollectedAt time.Time  `json:"collected_at"`
	Files       []FileItem `json:"files,omitempty"`
}

type Torrent struct {
	Infohash    string              `json:"infohash"`
	Name        string              `json:"name"`
	Length      int                 `json:"length"`
	Download    int                 `json:"download"`
	CollectedAt JsonTime            `json:"collected_at"`
	Files       []FileItem          `json:"files,omitempty"`
	Highlight   map[string][]string `json:"highlight,omitempty"`
}

func completionSuggest(ctx context.Context, text string, size int) (result []string, err error) {
	result = make([]string, 0, size)
	search := esClient.Search().Index(esIndex).Type(esType)
	query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "torrent"))
	suggester := elastic.NewCompletionSuggester("completion-suggest").
		Text(text).Field("name2").SkipDuplicates(true).Size(size)
	search = search.Query(query).Suggester(suggester)
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
	return
}

func termSuggest(ctx context.Context, text string, size int) (result []string, err error) {
	result = make([]string, 0, 1)
	search := esClient.Search().Index(esIndex).Type(esType)
	query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "torrent"))
	suggester := elastic.NewTermSuggester("term-suggest").
		Text(text).Field("name").Size(1).SuggestMode("popular")
	search = search.Query(query).Suggester(suggester)
	searchResult, err := search.Do(ctx)
	if err != nil {
		return
	}

	suggestResult := searchResult.Suggest["term-suggest"]
	if suggestResult == nil || len(suggestResult) == 0 {
		return
	}

	for _, suggest := range suggestResult {
		if len(result) >= size {
			break
		}

		if suggest.Options != nil && len(suggest.Options) > 0 {
			result = append(result, suggest.Options[0].Text)
		}
	}

	return
}

func EsSuggest(ctx context.Context, text string, size int) (result []string, err error) {
	_, seg := xray.BeginSubsegment(ctx, "es-suggest")
	defer seg.Close(err)

	input := strings.TrimSpace(text)
	if input == "" {
		result = make([]string, 0)
		return
	}

	result, err = completionSuggest(ctx, input, size)
	if err != nil || len(result) > 0 {
		return
	}

	result, err = termSuggest(ctx, input, size)
	return
}

func EsSearch(ctx context.Context, text string, offset int, limit int) (total int64, result []Torrent, err error) {
	_, seg := xray.BeginSubsegment(ctx, "es-search")
	defer seg.Close(err)

	result = make([]Torrent, 0, limit)
	if offset+limit > maxResultWindow {
		return
	}

	input := strings.TrimSpace(text)
	search := esClient.Search().Index(esIndex).Type(esType)
	if input == "" {
		query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "torrent"))
		search = search.Query(query)
	} else {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewTermQuery("type", "torrent"))
		boolQuery = boolQuery.Must(elastic.NewBoolQuery().Should(
			elastic.NewMatchQuery("name", input).Boost(1.0)))
		//elastic.NewMatchQuery("files.path", input).Boost(1.0)))

		query := elastic.NewFunctionScoreQuery().BoostMode("multiply")
		query = query.Query(boolQuery)

		hotFunction := elastic.NewFieldValueFactorFunction()
		hotFunction = hotFunction.Field("hot").Modifier("ln2p").Missing(1).Weight(0.1)
		collectFunction := elastic.NewGaussDecayFunction().FieldName("collected_at")
		collectFunction = collectFunction.Origin(time.Now()).Offset("20d").Scale("200d").Decay(0.5).Weight(0.1)
		query = query.AddScoreFunc(hotFunction).AddScoreFunc(collectFunction)

		highlight := elastic.NewHighlight().Field("name")
		search = search.Query(query).Highlight(highlight)
		search = search.Sort("_score", false)
	}

	search = search.From(offset).Size(limit)
	res, err := search.Do(ctx)
	if err != nil {
		return
	}

	total = res.TotalHits()
	if total > maxResultWindow {
		total = maxResultWindow
	}
	for _, hit := range res.Hits.Hits {
		item := EsTorrent{}
		err = json.Unmarshal(*hit.Source, &item)
		if err != nil {
			continue
		}

		t := Torrent{
			Infohash:    hit.Id,
			Name:        item.Name,
			Length:      item.Length,
			Download:    item.Download,
			CollectedAt: JsonTime{item.CollectedAt},
			Highlight:   hit.Highlight,
		}
		for _, f := range item.Files {
			if !strings.HasPrefix(f.Path[0], "_____") {
				t.Files = append(t.Files, f)
			}
		}
		result = append(result, t)
	}
	return
}

func EsUpdateMetaData(ctx context.Context, meta *updatePost) (err error) {
	_, seg := xray.BeginSubsegment(ctx, "es-update")
	defer seg.Close(err)

	_, err = esClient.Get().Index(esIndex).Type(esType).Id(meta.Infohash).Do(ctx)

	item := EsTorrent{}
	if err != nil {
		// not found
		err = nil
		if meta.Meta.Name != "" {
			item.Name = meta.Meta.Name
			item.Name2 = item.Name
			item.Files = meta.Meta.Files
			item.Download = meta.Hot
			item.CollectedAt = time.Now()
			item.Type = "torrent"

			for _, fileItem := range meta.Meta.Files {
				if !strings.HasPrefix(fileItem.Path[0], "_____") {
					item.Files = append(item.Files, fileItem)
				}
			}

			var total int
			for _, file := range meta.Meta.Files {
				total += file.Length
			}
			if total != 0 {
				item.Length = total
			} else {
				item.Length = meta.Meta.Length
			}

			_, err = esClient.Index().Index(esIndex).Type(
				esType).Id(meta.Infohash).BodyJson(item).Do(ctx)
			log.Info(meta.Infohash)
			log.Info("%+v", item)
		}
	} else {
		// found
		if meta.Hot != 0 {
			script := elastic.NewScript("ctx._source.hot=params.hot;ctx._source.collected_at=params.collected_at").Params(
				map[string]interface{}{
					"hot":          meta.Hot,
					"collected_at": time.Now(),
				},
			)
			_, err = esClient.Update().Index(esIndex).Type(esType).Id(meta.Infohash).Script(script).Do(ctx)
		}
	}

	return
}
