package server

import (
	"encoding/json"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/olivere/elastic"
	"strings"
	"time"
	log "github.com/alecthomas/log4go"
)

var esClient *elastic.Client

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

type Movie struct {
	Id         string               `json:"id"`
	Name       string               `json:"name"`
	Alias      string               `json:"alias"`
	Year       int                  `json:"year"`
	Genre      []string             `json:"genre,omitempty"`
	Poster     string               `json:"poster"`
	Slate      string               `json:"slate"`
	SlateCover string               `json:"slate_cover"`
	Desc       string               `json:"description"`
	RateCount  int                  `json:"rating_count"`
	RateValue  float32              `json:"rating_value"`
	Director   []string             `json:"director,omitempty"`
	Creator    []string             `json:"creator,omitempty"`
	Actor      []string             `json:"actor,omitempty"`
	Country    []string             `json:"country,omitempty"`
	Language   []string             `json:"language,omitempty"`
	Runtime    int                  `json:"runtime,omitempty"`
	Release    string               `json:"release,omitempty"`

	Highlight   map[string][]string `json:"highlight,omitempty"`

	Youtube    []*YoutubeItem       `json:"youtube,omitempty"`
	BT         []*TorrentItem       `json:"bt,omitempty"`
}

type MV struct {
	Id         string               `json:"id"`
	Name       string               `json:"name"`
	Slate      string               `json:"slate"`
	Poster     string               `json:"poster"`
	Genre      []string             `json:"genre,omitempty"`
	Runtime    int                  `json:"runtime,omitempty"`
	Hot        int                  `json:"hot,omitempty"`

	Highlight  map[string][]string `json:"highlight,omitempty"`
}

func (p *suggestParam) completionSuggest() (result []string, err error) {
	result = make([]string, 0, p.Size)
	search := esClient.Search().Index(esIndex).Type(esType)
	suggester := elastic.NewCompletionSuggester("completion-suggest").
		Text(p.Text).Field("name2").SkipDuplicates(true).Size(p.Size)
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
	result = make([]string, 0, 1)
	search := esClient.Search().Index(esIndex).Type(esType)
	suggester := elastic.NewTermSuggester("term-suggest").
		Text(p.Text).Field("name").Size(1).SuggestMode("popular")
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

		if suggest.Options != nil && len(suggest.Options) > 0 {
			result = append(result, suggest.Options[0].Text)
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

func (p *updateParam)UpdateTorrent() (err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "es-update")
	defer seg.Close(err)

	_, err = esClient.Get().Index(esIndex).Type(esType).Id(p.Infohash).Do(p.ctx)

	item := EsTorrent{}
	if err != nil {
		// not found
		err = nil
		if p.Meta.Name != "" {
			item.Name = p.Meta.Name
			item.Name2 = item.Name
			item.Files = p.Meta.Files
			item.Download = p.Hot
			item.CollectedAt = time.Now()
			item.Type = "torrent"

			for _, fileItem := range p.Meta.Files {
				if !strings.HasPrefix(fileItem.Path[0], "_____") {
					item.Files = append(item.Files, fileItem)
				}
			}

			var total int
			for _, file := range p.Meta.Files {
				total += file.Length
			}
			if total != 0 {
				item.Length = total
			} else {
				item.Length = p.Meta.Length
			}

			_, err = esClient.Index().Index(esIndex).Type(
				esType).Id(p.Infohash).BodyJson(item).Do(p.ctx)
			log.Info(p.Infohash)
			log.Info("%+v", item)
		}
	} else {
		// found
		if p.Hot != 0 {
			script := elastic.NewScript("ctx._source.hot=params.hot;ctx._source.collected_at=params.collected_at").Params(
				map[string]interface{}{
					"hot":          p.Hot,
					"collected_at": time.Now(),
				},
			)
			_, err = esClient.Update().Index(esIndex).Type(esType).Id(p.Infohash).Script(script).Do(p.ctx)
		}
	}

	return
}

func (p *searchParam) SearchBT() (total int64, result []*Torrent, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "torrent-search")
	defer seg.Close(err)

	result = make([]*Torrent, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	input := strings.TrimSpace(p.Text)
	search := esClient.Search().Index(esIndex).Type(esType)
	if input == "" {
		query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "torrent"))
		search = search.Query(query)
	} else {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewTermQuery("type", "torrent"))
		boolQuery = boolQuery.Must(elastic.NewBoolQuery().Should(
			elastic.NewMatchQuery("name", input).Boost(1.0),))
		//elastic.NewMatchQuery("files.path", input).Boost(1.0)))

		query := elastic.NewFunctionScoreQuery().BoostMode("multiply")
		query = query.Query(boolQuery)

		hotFunction := elastic.NewFieldValueFactorFunction()
		hotFunction = hotFunction.Field("hot").Modifier("ln2p").Missing(1).Weight(0.1)
		collectFunction := elastic.NewGaussDecayFunction().FieldName("collected_at")
		collectFunction = collectFunction.Origin(time.Now()).Offset("30d").Scale("365d").Decay(0.5).Weight(0.1)
		query = query.AddScoreFunc(hotFunction).AddScoreFunc(collectFunction)

		highlight := elastic.NewHighlight().Field("name")
		search = search.Query(query).Highlight(highlight)
		search = search.Sort("_score", false)
	}

	search = search.From(p.Offset).Size(p.Limit)
	res, err := search.Do(p.ctx)
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
		result = append(result, &t)
	}
	return
}


func (p *searchParam) SearchMovie() (total int64, result []*Movie, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "movie-search")
	defer seg.Close(err)

	result = make([]*Movie, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	input := strings.TrimSpace(p.Text)
	search := esClient.Search().Index(esIndex).Type(esType)
	if input == "" {
		query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "imdb"))
		search = search.Query(query)
	} else {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewTermQuery("type", "imdb"))
		boolQuery = boolQuery.Must(elastic.NewBoolQuery().Should(
			elastic.NewMatchQuery("name", input).Boost(2.0),
			elastic.NewMatchQuery("alias", input).Boost(2.0),
			elastic.NewMatchQuery("actor", input).Boost(1.0),
			elastic.NewMatchQuery("director", input).Boost(1.0),))

		query := elastic.NewFunctionScoreQuery().BoostMode("multiply")
		query = query.Query(boolQuery)

		hotFunction := elastic.NewFieldValueFactorFunction()
		hotFunction = hotFunction.Field("recommend").Modifier("ln2p").Missing(1).Weight(0.5)
		collectFunction := elastic.NewGaussDecayFunction().FieldName("release")
		collectFunction = collectFunction.Origin(time.Now()).Offset("120d").Scale("36500d").Decay(0.5).Weight(0.1)
		query = query.AddScoreFunc(hotFunction).AddScoreFunc(collectFunction)

		highlight := elastic.NewHighlight().Field("name")
		search = search.Query(query).Highlight(highlight)
		search = search.Sort("_score", false)
	}

	search = search.From(p.Offset).Size(p.Limit)
	res, err := search.Do(p.ctx)
	if err != nil {
		return
	}

	total = res.TotalHits()
	if total > maxResultWindow {
		total = maxResultWindow
	}
	filmIds := make([]string, 0, p.Limit)
	for _, hit := range res.Hits.Hits {
		item := Movie{}
		err = json.Unmarshal(*hit.Source, &item)
		if err != nil {
			continue
		}

		item.Highlight = hit.Highlight

		filmIds = append(filmIds, item.Id)
		result = append(result, &item)
	}

	btMap, _ := QueryTorrent(p.ctx, filmIds)
	youtubeMap, _ := QueryYoutube(p.ctx, filmIds)
	for _, item := range result {
		item.BT = btMap[item.Id]
		item.Youtube = youtubeMap[item.Id]
	}
	return
}

func (p *searchParam) SearchMV() (total int64, result []*MV, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "mv-search")
	defer seg.Close(err)

	result = make([]*MV, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	input := strings.TrimSpace(p.Text)
	search := esClient.Search().Index(esIndex).Type(esType)
	if input == "" {
		query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "mv"))
		search = search.Query(query)
	} else {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewTermQuery("type", "mv"))
		boolQuery = boolQuery.Must(elastic.NewBoolQuery().Should(
			elastic.NewMatchQuery("name", input).Boost(1.0),))

		highlight := elastic.NewHighlight().Field("name")
		search = search.Query(boolQuery).Highlight(highlight)
		search = search.Sort("_score", false)
	}

	search = search.From(p.Offset).Size(p.Limit)
	res, err := search.Do(p.ctx)
	if err != nil {
		return
	}

	total = res.TotalHits()
	if total > maxResultWindow {
		total = maxResultWindow
	}
	for _, hit := range res.Hits.Hits {
		item := MV{}
		err = json.Unmarshal(*hit.Source, &item)
		if err != nil {
			continue
		}

		item.Highlight = hit.Highlight
		result = append(result, &item)
	}

	return
}
