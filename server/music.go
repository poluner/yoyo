package server

import (
	"time"
	"strings"
	"encoding/json"
	"github.com/olivere/elastic"
	log "github.com/alecthomas/log4go"
	"github.com/aws/aws-xray-sdk-go/xray"
	"fmt"
)

type Singer struct {
	Id         string               `json:"id"`
	Type       string               `json:"type"`
	Title      string               `json:"title"`
	Poster     string               `json:"poster"`
	Slate      string               `json:"slate"`

	Highlight   map[string][]string `json:"highlight,omitempty"`
}

type Song struct {
	Id         string               `json:"id"`
	Type       string               `json:"type"`
	Title      string               `json:"title"`
	Poster     string               `json:"poster"`
	Slate      string               `json:"slate"`
	Language   []string             `json:"language"`
	Runtime    int                  `json:"runtime"`
	Release    string               `json:"release"`
	Lyric      string               `json:"lyric_url,omitempty"`
	Singer     []string             `json:"singer"`
	SingerId   []string             `json:"singer_id"`
	AlbumId    string               `json:"album_id"`
	AlbumTitle string               `json:"album_title"`
	Status     int                  `json:"status"`

	Highlight   map[string][]string `json:"highlight,omitempty"`
}

type Album struct {
	Id         string               `json:"id"`
	Type       string               `json:"type"`
	Title      string               `json:"title"`
	Description string              `json:"description,omitempty"`
	Poster     string               `json:"poster"`
	Slate      string               `json:"slate,omitempty"`
	Language   []string             `json:"language,omitempty"`
	Runtime    int                  `json:"runtime,omitempty"`
	Release    string               `json:"release"`
	SingerId   []string             `json:"singer_id,omitempty"`
	SongId     []string             `json:"song_id"`
	SongCount  int                  `json:"song_count"`
	Songs      []*Song              `json:"songs,omitempty"`

	Highlight   map[string][]string `json:"highlight,omitempty"`
}

func (p *searchParam) SearchSong() (total int64, result []*Song, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "song-search")
	defer seg.Close(err)

	result = make([]*Song, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	input := strings.TrimSpace(p.Text)
	search := esClient.Search().Index(songIndex).Type(songType)
	if input != "" {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewTermQuery("type", "song"))
		boolQuery = boolQuery.Must(elastic.NewBoolQuery().Should(
			elastic.NewMatchQuery("title", input)))
		highlight := elastic.NewHighlight().Field("title")
		search = search.Query(boolQuery).Highlight(highlight)
		search = search.Sort("_score", false)
	} else if p.Singer != "" {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewTermQuery("type", "song"))
		boolQuery = boolQuery.Must(elastic.NewTermQuery("singer_id", p.Singer))
		search = search.Query(boolQuery)
		search = search.Sort("title.keyword", true)
	} else {
		query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "song"))
		search = search.Query(query)
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
		item := Song{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		item.Highlight = hit.Highlight
		result = append(result, &item)
	}

	return
}

func (p *searchParam) SearchAlbum() (total int64, result []*Album, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "album-search")
	defer seg.Close(err)

	result = make([]*Album, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	input := strings.TrimSpace(p.Text)
	search := esClient.Search().Index(songIndex).Type(songType)
	if input != "" {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewBoolQuery().
			Should(elastic.NewTermQuery("type", "playlist")).
			Should(elastic.NewTermQuery("type", "album")))
		boolQuery = boolQuery.Must(elastic.NewBoolQuery().Should(
			elastic.NewMatchQuery("title", input)))
		highlight := elastic.NewHighlight().Field("title")
		search = search.Query(boolQuery).Highlight(highlight)
		search = search.Sort("_score", false)
	} else if p.Singer != "" {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewTermQuery("type", "album"))
		boolQuery = boolQuery.Must(elastic.NewTermQuery("singer_id", p.Singer))
		search = search.Query(boolQuery)
		search = search.Sort("title.keyword", true)
	} else {
		query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "album"))
		search = search.Query(query)
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
		item := Album{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		item.Highlight = hit.Highlight
		result = append(result, &item)
	}

	return
}

func (p *discoverParam) DiscoverAlbum() (total int64, result []*Album, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "album-discover")
	defer seg.Close(err)

	result = make([]*Album, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	search := esClient.Search().Index(songIndex).Type(songType)

	query := elastic.NewBoolQuery()
	query = query.Must(elastic.NewTermQuery("type", "album"))
	if p.Language != "" {
		query = query.Must(elastic.NewTermQuery("language", p.Language))
	}
	search = search.Query(query)
	search = search.Sort("title.keyword", p.Ascend == 1)

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
		item := Album{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		result = append(result, &item)
	}

	return
}

func (p *getParam) GetAlbum() (result *Album, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "album-get")
	defer seg.Close(err)

	cache, e := redisConn.Get(p.Id).Bytes()
	if e == nil && len(cache) > 0 {
		album := Album{}
		err = json.Unmarshal(cache, &album)
		if err != nil {
			return
		}
		result = &album
		return
	}

	get := esClient.Get().Index(songIndex).Type(songType).Id(p.Id)
	res, err := get.Do(p.ctx)
	if err != nil {
		return
	}
	if res.Source == nil {
		return
	}

	album := Album{}
	err = json.Unmarshal(*res.Source, &album)
	if err != nil {
		return
	}

	if album.SongId != nil && len(album.SongId) > 0 {
		mget := esClient.Mget()
		for _, id := range album.SongId {
			item := elastic.NewMultiGetItem().Index(songIndex).Type(songType).Id(id)
			mget = mget.Add(item)
		}

		songs := make([]*Song, 0, len(album.SongId))
		res, e := mget.Do(p.ctx)
		log.Error(e)
		if e == nil {
			for _, hit := range res.Docs {
				if hit.Source == nil {
					continue
				}

				item := Song{}
				e := json.Unmarshal(*hit.Source, &item)
				if e != nil {
					continue
				}
				log.Info("%+v", item)
				songs = append(songs, &item)
			}
		}
		log.Info(songs)
		album.Songs = songs
	}

	cache, e = json.Marshal(&album)
	if e == nil {
		redisConn.Set(p.Id, cache, time.Hour * 2)
	}
	result = &album
	return
}

func (p *mgetParam) GetAlbums() (result map[string]*Album, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "album-mget")
	defer seg.Close(err)

	result = make(map[string]*Album)
	if p.Ids == nil || len(p.Ids) == 0 {
		return
	}

	mget := esClient.Mget()
	for _, id := range p.Ids {
		item := elastic.NewMultiGetItem().Index(songIndex).Type(songType).Id(id)
		mget = mget.Add(item)
	}

	res, err := mget.Do(p.ctx)
	if err != nil {
		return
	}

	for _, hit := range res.Docs {
		if hit.Source == nil {
			continue
		}

		item := Album{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}
		result[item.Id] = &item
	}

	return
}

func (p *mgetParam) GetSongs() (result map[string]*Song, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "song-mget")
	defer seg.Close(err)

	result = make(map[string]*Song)
	if p.Ids == nil || len(p.Ids) == 0 {
		return
	}

	mget := esClient.Mget()
	for _, id := range p.Ids {
		item := elastic.NewMultiGetItem().Index(songIndex).Type(songType).Id(id)
		mget = mget.Add(item)
	}

	res, err := mget.Do(p.ctx)
	if err != nil {
		return
	}

	for _, hit := range res.Docs {
		if hit.Source == nil {
			continue
		}

		item := Song{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}
		result[item.Id] = &item
	}

	return
}

func (p *searchParam) SearchSinger() (total int64, result []*Singer, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "singer-search")
	defer seg.Close(err)

	result = make([]*Singer, 0, p.Limit)
	if p.Offset + p.Limit > maxResultWindow {
		return
	}

	input := strings.TrimSpace(p.Text)
	search := esClient.Search().Index(songIndex).Type(songType)
	if input != "" {
		boolQuery := elastic.NewBoolQuery()
		boolQuery = boolQuery.Must(elastic.NewTermQuery("type", "singer"))
		boolQuery = boolQuery.Must(elastic.NewBoolQuery().Should(
			elastic.NewMatchQuery("title", input)))
		highlight := elastic.NewHighlight().Field("title")
		search = search.Query(boolQuery).Highlight(highlight)
		search = search.Sort("_score", false)
	} else {
		query := elastic.NewBoolQuery().Must(elastic.NewTermQuery("type", "singer"))
		search = search.Query(query)
		search = search.Sort("title.keyword", true)
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
		item := Singer{}
		e := json.Unmarshal(*hit.Source, &item)
		if e != nil {
			continue
		}

		item.Highlight = hit.Highlight
		result = append(result, &item)
	}

	return
}


func (p *getParam) GetCollection() (result *Album, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "collection-get")
	defer seg.Close(err)

	cache, e := redisConn.Get(p.Id).Bytes()
	if e == nil && len(cache) > 0 {
		album := Album{}
		err = json.Unmarshal(cache, &album)
		if err != nil {
			return
		}
		result = &album
		return
	}

	records, err := QueryCollections(p.ctx, []string{p.Id})
	if err != nil || len(records) == 0 {
		return
	}

	record := records[0]
	year, month, date := record.CreatedAt.Date()
	songs := strings.Split(record.SongId, ",")
	album := Album{
		Id: fmt.Sprintf("%d", record.Id),
		Type: "collection",
		Title: record.Title,
		Description: record.Description,
		Poster: record.Poster,
		Release: fmt.Sprintf("%d-%d-%d", year, month, date),
		SongId: songs,
		SongCount: len(songs),
	}

	if songs != nil && len(songs) > 0 {
		mget := esClient.Mget()
		for _, id := range songs {
			item := elastic.NewMultiGetItem().Index(songIndex).Type(songType).Id(id)
			mget = mget.Add(item)
		}

		songs := make([]*Song, 0, len(album.SongId))
		res, e := mget.Do(p.ctx)
		if e == nil {
			for _, hit := range res.Docs {
				if hit.Source == nil {
					continue
				}

				item := Song{}
				e := json.Unmarshal(*hit.Source, &item)
				if e != nil {
					continue
				}
				songs = append(songs, &item)
			}
		}
		album.Songs = songs
	}

	cache, e = json.Marshal(&album)
	if e == nil {
		redisConn.Set(p.Id, cache, time.Hour * 2)
	}
	result = &album
	return
}

func (p *mgetParam) GetCollections() (result map[string]*Album, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "collection-mget")
	defer seg.Close(err)

	result = make(map[string]*Album)
	if p.Ids == nil || len(p.Ids) == 0 {
		return
	}

	records, err := QueryCollections(p.ctx, p.Ids)
	if err != nil {
		return
	}

	for _, record := range records {
		year, month, date := record.CreatedAt.Date()
		songs := strings.Split(record.SongId, ",")
		item := Album{
			Id: fmt.Sprintf("%d", record.Id),
			Type: "collection",
			Title: record.Title,
			Description: record.Description,
			Poster: record.Poster,
			Release: fmt.Sprintf("%d-%d-%d", year, month, date),
			SongId: songs,
			SongCount: len(songs),
		}
		result[item.Id] = &item
	}

	return
}

func (p *getParam) GetSongUrl() (result map[string]string, err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "song-url-get")
	defer seg.Close(err)

	result, err = QuerySongUrl(p.ctx, p.Id)
	return
}