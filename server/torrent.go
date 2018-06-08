package server

import (
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
	"fmt"
)

var (
	uploader *s3manager.Uploader
)

func (m *metaInfo) insertEs(ctx context.Context, infohash string, hot int) (err error) {
	item := EsTorrent{}
	item.Name = m.Name
	item.Name2 = item.Name
	item.Download = hot
	item.CollectedAt = time.Now()
	item.Type = "torrent"

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

	_, err = esClient.Index().Index(esIndex).Type(
		esType).Id(infohash).BodyJson(item).Do(ctx)
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

	_, e := esClient.Get().Index(esIndex).Type(esType).Id(p.Infohash).Do(p.ctx)

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
			_, err = esClient.Update().Index(esIndex).Type(esType).Id(p.Infohash).Script(script).Do(p.ctx)
		}
	}

	return
}

func Upload(ctx context.Context, infohash string, data []byte) (err error) {
	_, seg := xray.BeginSubsegment(ctx, "upload-torrent")
	defer seg.Close(err)

	_, e := esClient.Get().Index(esIndex).Type(esType).Id(infohash).Do(ctx)
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
