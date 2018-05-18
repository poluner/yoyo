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

// mv和imdb类型
type Resource struct {
	Id         string               `json:"id"`
	Type       string               `json:"type"`
	Title      string               `json:"title"`
	Alias      string               `json:"alias,omitempty"`
	Year       int                  `json:"year,omitempty"`
	Genre      []string             `json:"genre,omitempty"`
	Poster     string               `json:"poster"`
	Slate      string               `json:"slate"`
	SlateCover string               `json:"slate_cover,omitempty"`
	Desc       string               `json:"description,omitempty"`
	RateCount  int                  `json:"rating_count,omitempty"`
	RateValue  float32              `json:"rating_value,omitempty"`
	Director   []string             `json:"director,omitempty"`
	Creator    []string             `json:"creator,omitempty"`
	Actor      []string             `json:"actor,omitempty"`
	Country    []string             `json:"country,omitempty"`
	Language   []string             `json:"language,omitempty"`
	Runtime    int                  `json:"runtime,omitempty"`
	Release    JsonTime             `json:"release,omitempty"`
	Hot        int                  `json:"hot,omitempty"`
	Cracked    bool                 `json:"cracked,omitempty"`
	Youtube    []YoutubeItem        `json:"youtube,omitempty"`
	BT         []TorrentItem        `json:"bt,omitempty"`

	Highlight   map[string][]string `json:"highlight,omitempty"`
}

func (p *suggestParam) completionSuggest() (result []string, err error) {
	result = make([]string, 0, p.Size)
	search := esClient.Search().Index(esIndex).Type(esType)

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
	result = make([]string, 0, 1)
	search := esClient.Search().Index(esIndex).Type(esType)

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
			item.Download = p.Hot
			item.CollectedAt = time.Now()
			item.Type = "torrent"

			for _, fileItem := range p.Meta.Files {
				if !strings.HasPrefix(fileItem.Path[0], "_____") {
					item.Files = append(item.Files, fileItem)
				}

				if fileItem.Path[0] == "Ultra XVid Codec Pack.exe " {
					return
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

func (p *searchParam) SearchMovie() (total int64, result []*Resource, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "movie-search")
	defer seg.Close(err)

	result = make([]*Resource, 0, p.Limit)
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
			elastic.NewMatchQuery("title", input).Boost(2.0),
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

		highlight := elastic.NewHighlight().Field("title")
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
		item := Resource{}
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

func (p *searchParam) SearchMV() (total int64, result []*Resource, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "mv-search")
	defer seg.Close(err)

	result = make([]*Resource, 0, p.Limit)
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
			elastic.NewMatchQuery("title", input).Boost(1.0),))

		highlight := elastic.NewHighlight().Field("title")
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
		item := Resource{}
		err = json.Unmarshal(*hit.Source, &item)
		if err != nil {
			continue
		}

		item.Highlight = hit.Highlight

		if item.Genre != nil && len(item.Genre) > 0 {
			if item.Genre[0] == "youtube" {
				item.Cracked = true
			}
		}
		result = append(result, &item)
	}

	return
}

func (p *discoverParam) Discover() (total int64, result []*Resource, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "movie-discover")
	defer seg.Close(err)

	result = make([]*Resource, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	search := esClient.Search().Index(esIndex).Type(esType)

	query := elastic.NewBoolQuery()
	query = query.Must(elastic.NewTermQuery("type", "imdb"))

	var hasCondition bool
	conditionQuery := elastic.NewBoolQuery()
	if p.Year != 0 {
		conditionQuery.Should(elastic.NewTermQuery("year", p.Year))
		hasCondition = true
	}
	if p.Language != "" {
		conditionQuery.Should(elastic.NewTermQuery("language", p.Language))
		hasCondition = true
	}
	if p.Country != "" {
		conditionQuery.Should(elastic.NewTermQuery("country", p.Country))
		hasCondition = true
	}
	if p.Genre != "" {
		conditionQuery.Should(elastic.NewTermQuery("genre", p.Genre))
		hasCondition = true
	}
	if hasCondition {
		query.Must(conditionQuery)
	}

	search = search.Query(query)
	if p.Sort == "release" {
		search = search.Sort("release", p.Ascend == 1)
	} else if p.Sort == "rating_value" {
		search = search.Sort("rating_value", p.Ascend == 1)
	} else if p.Sort == "rating_count" {
		search = search.Sort("rating_count", p.Ascend == 1)
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
		item := Resource{}
		err = json.Unmarshal(*hit.Source, &item)
		if err != nil {
			continue
		}

		result = append(result, &item)
	}

	return
}

func (p *getParam) GetMovie() (result *Resource, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "movie-get")
	defer seg.Close(err)

	cache, e := redisConn.Get(p.Id).Bytes()
	if e == nil && len(cache) > 0 {
		movie := Resource{}
		err = json.Unmarshal(cache, &movie)
		if err != nil {
			return
		}
		result = &movie
		return
	}

	search := esClient.Search().Index(esIndex).Type(esType)
	query := elastic.NewTermQuery("id", p.Id)
	search = search.Query(query)
	res, err := search.Do(p.ctx)
	if err != nil {
		return
	}
	if len(res.Hits.Hits) == 0 {
		return
	}

	movie := Resource{}
	hit := res.Hits.Hits[0]
	err = json.Unmarshal(*hit.Source, &movie)
	if err != nil {
		return
	}

	filmIds := []string{p.Id}
	btMap, _ := QueryTorrent(p.ctx, filmIds)
	youtubeMap, _ := QueryYoutube(p.ctx, filmIds)
	movie.BT = btMap[p.Id]
	movie.Youtube = youtubeMap[p.Id]

	cache, e = json.Marshal(&movie)
	if e == nil {
		redisConn.Set(p.Id, cache, time.Hour * 2)
	}
	result = &movie
	return
}

func (p *getParam) GetMV() (result *Resource, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "mv-get")
	defer seg.Close(err)

	cache, e := redisConn.Get(p.Id).Bytes()
	if e == nil && len(cache) > 0 {
		mv := Resource{}
		err = json.Unmarshal(cache, &mv)
		if err != nil {
			return
		}
		result = &mv
		return
	}

	search := esClient.Search().Index(esIndex).Type(esType)
	query := elastic.NewTermQuery("id", p.Id)
	search = search.Query(query)
	res, err := search.Do(p.ctx)
	if err != nil {
		return
	}
	if len(res.Hits.Hits) == 0 {
		return
	}

	mv := Resource{}
	hit := res.Hits.Hits[0]
	err = json.Unmarshal(*hit.Source, &mv)
	if err != nil {
		return
	}

	if mv.Genre != nil && len(mv.Genre) > 0 {
		if mv.Genre[0] == "youtube" {
			mv.Cracked = true
		}
	}

	cache, e = json.Marshal(&mv)
	if e == nil {
		redisConn.Set(p.Id, cache, time.Hour * 2)
	}
	result = &mv
	return
}

func (p *mgetParam) MGet() (result []*Resource, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "resource-mget")
	defer seg.Close(err)

	result = make([]*Resource, 0, 10)
	if p.Ids == nil || len(p.Ids) == 0 {
		return
	}

	search := esClient.Search().Index(esIndex).Type(esType)
	query := elastic.NewBoolQuery()
	for _, id := range p.Ids {
		query = query.Should(elastic.NewTermQuery("id", id))
	}

	search = search.Query(query)
	res, err := search.Do(p.ctx)
	if err != nil {
		return
	}
	if len(res.Hits.Hits) == 0 {
		return
	}

	for _, hit := range res.Hits.Hits {
		item := Resource{}
		err = json.Unmarshal(*hit.Source, &item)
		if err != nil {
			continue
		}

		if item.Type == "mv" && item.Genre != nil && len(item.Genre) > 0 {
			if item.Genre[0] == "youtube" {
				item.Cracked = true
			}
		}

		result = append(result, &item)
	}

	return
}
