package server

import (
	"fmt"
	"time"
	"errors"
	"bytes"
	"context"
	"strings"
	"github.com/olivere/elastic"
	"github.com/LiuRoy/yoyo/dht"
	log "github.com/alecthomas/log4go"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/aws/aws-sdk-go/service/s3/s3manager"
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
	TorrentUrl  string              `json:"torrent_url,omitempty"`
	Files       []FileItem          `json:"files,omitempty"`
	Highlight   map[string][]string `json:"highlight,omitempty"`
}

func (m *metaInfo) insertEs(ctx context.Context, infohash string, hot int) (err error) {
	item := EsTorrent{}
	item.Name = m.Name
	item.Name2 = item.Name
	item.Download = hot
	item.CollectedAt = time.Now()
	item.Type = "torrent"

	// 不添加可执行文件
	name := strings.ToLower(item.Name)
	if strings.HasSuffix(name, ".exe") || strings.HasSuffix(name, ".iso") {
		return
	}

	// 删除一些软件
	if strings.Contains(name, "microsoft") ||
		strings.Contains(name, "adobe") ||
			strings.Contains(name, "keygen") ||
				strings.Contains(name, "cracked") ||
					strings.Contains(name, "setup") {
		return
	}

	for _, fileItem := range m.Files {
		if !strings.HasPrefix(fileItem.Path[0], "_____") {
			item.Files = append(item.Files, fileItem)
		}

		if fileItem.Path[0] == "Ultra XVid Codec Pack.exe " {
			return
		}
	}

	var total int
	for _, file := range m.Files {
		total += file.Length
	}
	if total != 0 {
		item.Length = total
	} else {
		item.Length = m.Length
	}

	_, err = esClient.Index().Index(resourceIndex).Type(
		resourceType).Id(infohash).BodyJson(item).Do(ctx)
	log.Info(infohash)
	log.Info("%+v", item)
	return
}

func parseTorrent(data []byte) (meta metaInfo, infohash string, err error) {
	torrent, err := dht.Decode(data)
	if err != nil {
		return
	}

	torrentMap := torrent.(map[string]interface{})
	info, ok := torrentMap["info"]
	if !ok {
		err = errors.New("no info key")
		return
	}

	infoMap := info.(map[string]interface{})
	name, ok := infoMap["name"]
	if !ok {
		err = errors.New("no name key")
		return
	}

	meta.Name = name.(string)
	if v, ok := infoMap["files"]; ok {
		files := v.([]interface{})
		meta.Files = make([]FileItem, len(files))

		for i, item := range files {
			f := item.(map[string]interface{})

			length := f["length"].(int)
			path := f["path"].([]interface{})

			meta.Files[i] = FileItem{
				Path:   make([]string, len(path)),
				Length: length,
			}

			for j, dir := range path {
				meta.Files[i].Path[j] = dir.(string)
			}
		}
	} else if _, ok := infoMap["length"]; ok {
		meta.Length = infoMap["length"].(int)
	} else {
		err = errors.New("no files or length key")
	}
	return
}

func (p *updateParam) UpdateTorrent() (err error) {
	_, seg := xray.BeginSubsegment(p.ctx, "es-update")
	defer seg.Close(err)

	_, e := esClient.Get().Index(resourceIndex).Type(resourceType).Id(p.Infohash).Do(p.ctx)

	if e != nil {
		// not found
		if p.Meta.Name != "" {
			err = p.Meta.insertEs(p.ctx, p.Infohash, p.Hot)
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
			_, err = esClient.Update().Index(resourceIndex).Type(resourceType).Id(p.Infohash).Script(script).Do(p.ctx)
		}
	}

	return
}

func Upload(ctx context.Context, infohash string, data []byte) (err error) {
	_, seg := xray.BeginSubsegment(ctx, "upload-torrent")
	defer seg.Close(err)

	_, e := esClient.Get().Index(resourceIndex).Type(resourceType).Id(infohash).Do(ctx)
	if e != nil {
		// 解析种子并把种子放入es中
		meta, infohash, ee := parseTorrent(data)
		log.Info(infohash)
		if ee != nil {
			err  = ee
			return
		}

		ee = meta.insertEs(ctx, infohash, 1)
		if ee != nil {
			log.Error("infohash: %s, meta: %+v insert failed", infohash, meta)
		}
	}

	key := fmt.Sprintf("%s.torrent", infohash)
	_, err = uploader.UploadWithContext(ctx, &s3manager.UploadInput{
		Bucket: &s3bucket,
		Key: &key,
		Body:   bytes.NewReader(data),
	})
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
	search := esClient.Search().Index(resourceIndex).Type(resourceType)
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
		sizeFunction := elastic.NewGaussDecayFunction().FieldName("length")
		sizeFunction = sizeFunction.Origin(750000000).Offset(250000000).Scale(100000000000).Decay(0.5).Weight(1)
		query = query.AddScoreFunc(hotFunction).AddScoreFunc(collectFunction).AddScoreFunc(sizeFunction)

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

	infohashs := make([]string, 0, p.Limit)
	for _, hit := range res.Hits.Hits {
		infohashs = append(infohashs, hit.Id)
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
			CollectedAt: JsonTime{item.CollectedAt},
			Highlight:   hit.Highlight,
		}
		if p.IgnoreFiles == 0 {
			for _, f := range item.Files {
				if !strings.HasPrefix(f.Path[0], "_____") {
					t.Files = append(t.Files, f)
				}
			}
		}
		result = append(result, &t)
	}

	downloadMap, _ := QueryTorrentUrl(p.ctx, infohashs)
	for _, item := range result {
		item.TorrentUrl = downloadMap[item.Infohash]
	}
	return
}
