package server

import (
	"strings"
	"time"
	"regexp"
	"context"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/olivere/elastic"
)

var (
	sizeFormatPattern = regexp.MustCompile(`\._V1.*\.jpg$`)
)

// mv和imdb类型
type Resource struct {
	Id         string               `json:"id"`
	Type       string               `json:"type"`
	PType      string               `json:"p_type,omitempty"`
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
	Release    string               `json:"release,omitempty"`
	Hot        int                  `json:"hot,omitempty"`
	Cracked    bool                 `json:"cracked,omitempty"`
	Youtube    []YoutubeItem        `json:"youtube,omitempty"`
	BT         []Torrent            `json:"bt,omitempty"`
	Video      []VideoItem          `json:"video,omitempty"`

	Highlight   map[string][]string `json:"highlight,omitempty"`
}

// 爬取的海报链接不是高清图片,需要转成高清链接
func imdbPoster(poster string) string {
	return sizeFormatPattern.ReplaceAllString(poster, ".jpg")
}

// 爬取的海报链接不是高清图片,需要转成高清链接
func youtubePoster(poster string) string {
	return strings.Replace(poster, "maxresdefault", "mqdefault", 1)
}

// 网站名称的第一个小写字母变大写
func mvSiteProcess(site string) string {
	if site == "" {
		return site
	}

	tmp := []byte(site)
	if tmp[0] >= 'a' && tmp[0] <= 'z' {
		tmp[0] = tmp[0] - 32
	}
	return string(tmp)
}

func multiGetBT(ctx context.Context, infohashs []string) (result []Torrent, err error) {
	_, seg := xray.BeginSubsegment(ctx, "torrent-mget")
	defer seg.Close(err)

	result = make([]Torrent, 0, len(infohashs))
	if infohashs == nil || len(infohashs) == 0 {
		return
	}

	mget := esClient.Mget()
	for _, id := range infohashs {
		item := elastic.NewMultiGetItem().Index(resourceIndex).Type(resourceType).Id(id)
		mget = mget.Add(item)
	}

	res, err := mget.Do(ctx)
	if err != nil {
		return
	}

	downloadMap, _ := QueryTorrentUrl(ctx, infohashs)
	for _, hit := range res.Docs {
		item := EsTorrent{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		t := Torrent{
			Infohash:    hit.Id,
			Name:        item.Name,
			Length:      item.Length,
			Download:    item.Download,
			TorrentUrl:  downloadMap[hit.Id],
			CollectedAt: JsonTime{item.CollectedAt},
		}
		result = append(result, t)
	}
	return
}

func (p *searchParam) SearchMovie() (total int64, result []*Resource, maxScore float64, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "movie-search")
	defer seg.Close(err)

	result = make([]*Resource, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	input := strings.TrimSpace(p.Text)
	search := esClient.Search().Index(resourceIndex).Type(resourceType)
	if input == "" {
		query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "imdb"))
		search = search.Query(query)
	} else {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewTermQuery("type", "imdb"))
		boolQuery = boolQuery.Must(elastic.NewBoolQuery().Should(
			elastic.NewMatchQuery("title", input).Boost(10.0),
			elastic.NewMatchQuery("actor", input).Boost(1.0),
			elastic.NewMatchQuery("director", input).Boost(1.0),))

		query := elastic.NewFunctionScoreQuery().BoostMode("sum")
		query = query.Query(boolQuery)

		recommendFunction := elastic.NewFieldValueFactorFunction()
		recommendFunction = recommendFunction.Field("recommend").Modifier("ln2p").Missing(0).Weight(2.0)
		ratingCountFunction := elastic.NewFieldValueFactorFunction()
		ratingCountFunction = ratingCountFunction.Field("rating_count").Modifier("ln2p").Missing(0).Weight(2.0)
		query = query.AddScoreFunc(recommendFunction).AddScoreFunc(ratingCountFunction)

		highlight := elastic.NewHighlight().Field("title")
		search = search.Query(query).Highlight(highlight)
		search = search.Sort("_score", false)

		search = search.MinScore(1.5)
	}

	search = search.From(p.Offset).Size(p.Limit)
	res, err := search.Do(p.ctx)
	if err != nil {
		return
	}

	if res.Hits != nil && res.Hits.MaxScore != nil {
		maxScore = *(res.Hits.MaxScore)
	}
	total = res.TotalHits()
	if total > maxResultWindow {
		total = maxResultWindow
	}
	filmIds := make([]string, 0, p.Limit)
	for _, hit := range res.Hits.Hits {
		item := Resource{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		item.Highlight = hit.Highlight

		filmIds = append(filmIds, item.Id)
		result = append(result, &item)
	}

	youtubeMap, _ := QueryYoutube(p.ctx, filmIds)
	for _, item := range result {
		item.Youtube = youtubeMap[item.Id]

		item.Poster = imdbPoster(item.Poster)
		item.SlateCover = imdbPoster(item.SlateCover)
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
	search := esClient.Search().Index(resourceIndex).Type(resourceType)
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
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		item.Highlight = hit.Highlight

		if item.Genre != nil && len(item.Genre) > 0 {
			item.Cracked = crackedSite[item.Genre[0]]
			item.Genre[0] = mvSiteProcess(item.Genre[0])
		}
		item.Poster = youtubePoster(item.Poster)
		result = append(result, &item)
	}

	return
}

func (p *discoverParam) DiscoverMovie() (total int64, result []*Resource, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "movie-discover")
	defer seg.Close(err)

	result = make([]*Resource, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	search := esClient.Search().Index(resourceIndex).Type(resourceType)

	query := elastic.NewBoolQuery()
	query = query.Must(elastic.NewTermQuery("type", "imdb"))

	if p.Year != 0 {
		query.Must(elastic.NewTermQuery("year", p.Year))
	}
	if p.Language != "" {
		query.Must(elastic.NewTermQuery("language", p.Language))
	}
	if p.Country != "" {
		query.Must(elastic.NewTermQuery("country", p.Country))
	}
	if p.Genre != "" {
		query.Must(elastic.NewTermQuery("genre", p.Genre))
	}

	if p.Sort == "release" {
		query = query.Must(elastic.NewRangeQuery("release").Lt(time.Now()))
		search = search.Query(query)
		search = search.Sort("release", p.Ascend == 1)
	} else {
		functionQuery := elastic.NewFunctionScoreQuery().BoostMode("multiply")
		rateValueFunc := elastic.NewFieldValueFactorFunction()
		rateValueFunc = rateValueFunc.Field("rating_value").Missing(0.1)
		rateCountFunc := elastic.NewFieldValueFactorFunction()
		rateCountFunc = rateCountFunc.Field("rating_count").Missing(1)
		functionQuery = functionQuery.Query(query).AddScoreFunc(rateValueFunc).AddScoreFunc(rateCountFunc)
		search = search.Query(functionQuery)
		search = search.Sort("_score", p.Ascend == 1)
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
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		item.Poster = imdbPoster(item.Poster)
		item.SlateCover = imdbPoster(item.SlateCover)
		result = append(result, &item)
	}

	return
}

func (p *getParam) GetResource() (result *Resource, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "resource-get")
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

	get := esClient.Get().Index(resourceIndex).Type(resourceType).Id(p.Id)
	res, err := get.Do(p.ctx)
	if err != nil {
		return
	}
	if res.Source == nil {
		return
	}

	resource := Resource{}
	err = json.Unmarshal(*res.Source, &resource)
	if err != nil {
		return
	}

	if resource.Type == "imdb" {
		filmIds := []string{p.Id}
		youtubeMap, _ := QueryYoutube(p.ctx, filmIds)
		videoMap, _ := QueryVideo(p.ctx, filmIds)
		resource.Youtube = youtubeMap[p.Id]

		infohashs, _ := QueryTorrent(p.ctx, p.Id)
		resource.BT, _ = multiGetBT(p.ctx, infohashs)

		resource.Poster = imdbPoster(resource.Poster)
		resource.SlateCover = imdbPoster(resource.SlateCover)

		for _, v := range videoMap[p.Id] {
			if v.Cover != resource.SlateCover {
				resource.Video = append(resource.Video, v)
			}
		}
	} else if resource.Type == "mv" {
		if resource.Genre != nil && len(resource.Genre) > 0 {
			resource.Cracked = crackedSite[resource.Genre[0]]
			resource.Genre[0] = mvSiteProcess(resource.Genre[0])
		}
		resource.Poster = youtubePoster(resource.Poster)
	}

	cache, e = json.Marshal(&resource)
	if e == nil {
		redisConn.Set(p.Id, cache, time.Hour * 2)
	}
	result = &resource
	return
}

func (p *mgetParam) GetResources() (result map[string]*Resource, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "resource-mget")
	defer seg.Close(err)

	result = make(map[string]*Resource)
	if p.Ids == nil || len(p.Ids) == 0 {
		return
	}

	mget := esClient.Mget()
	for _, id := range p.Ids {
		item := elastic.NewMultiGetItem().Index(resourceIndex).Type(resourceType).Id(id)
		mget = mget.Add(item)
	}

	res, err := mget.Do(p.ctx)
	if err != nil {
		return
	}

	youtubeMap, _ := QueryYoutube(p.ctx, p.Ids)
	for _, hit := range res.Docs {
		if hit.Source == nil {
			continue
		}

		item := Resource{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		if item.Type == "mv" {
			if item.Genre != nil && len(item.Genre) > 0 {
				item.Cracked = crackedSite[item.Genre[0]]
				item.Genre[0] = mvSiteProcess(item.Genre[0])
			}
			item.Poster = youtubePoster(item.Poster)
		} else if item.Type == "imdb" {
			item.Youtube = youtubeMap[item.Id]
			item.Poster = imdbPoster(item.Poster)
			item.SlateCover = imdbPoster(item.SlateCover)
		}

		result[item.Id] = &item
	}

	return
}
