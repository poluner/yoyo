package server

import (
	"fmt"
	"time"
	"context"
)

type SongDownload struct {
	Id          uint64    `gorm:"column:id;type:bigint;primary_key"`
	SongId      string    `gorm:"column:song_id;type:varchar(20)"`
	BitRate     string    `gorm:"column:bit_rate;type:varchar(20)"`
	DownloadUrl string    `gorm:"column:download_url;type:varchar(300)"`
	CreatedAt   time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
}

func (SongDownload) TableName() string {
	return "song_download"
}

type SongCollection struct {
	Id          uint64    `gorm:"column:id;type:bigint;primary_key"`
	Title       string    `gorm:"column:title;type:varchar(100)"`
	Description string    `gorm:"column:description;type:varchar(500)"`
	Poster      string    `gorm:"column:poster;type:varchar(300)"`
	SongId      string    `gorm:"column:song_id;type:varchar(5000)"`
	CreatedAt   time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
}

func (SongCollection) TableName() string {
	return "song_collection"
}

type TorrentDownload struct {
	Id          uint64    `gorm:"column:id;type:bigint;primary_key"`
	InfoHash    string    `gorm:"column:infohash;type:varchar(40)"`
	DownloadUrl string    `gorm:"column:download_url;type:varchar(300)"`
	Kind        int8      `gorm:"column:kind;type:tinyint"`
	CreatedAt   time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
}

func (TorrentDownload) TableName() string {
	return "torrent"
}

type IMDBTorrent struct {
	Id        uint64    `gorm:"column:id;type:bigint;primary_key"`
	FilmId    string    `gorm:"column:film_id;type:varchar(20)"`
	InfoHash  string    `gorm:"column:infohash;type:varchar(40)"`
	BtName    string    `gorm:"column:bt_name;type:varchar(300)"`
	Length    int64     `gorm:"column:length;type:bigint"`
	CreatedAt time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
}

func (IMDBTorrent) TableName() string {
	return "imdb_torrent"
}

type IMDBYoutube struct {
	Id        uint64    `gorm:"column:id;type:bigint;primary_key"`
	FilmId    string    `gorm:"column:film_id;type:varchar(20)"`
	VideoName string    `gorm:"column:video_name;type:varchar(300)"`
	PlayId    string    `gorm:"column:play_id;type:varchar(20)"`
	Cover     string    `gorm:"column:cover;type:varchar(100)"`
	Duration  int32     `gorm:"column:duration;type:int"`
	ViewCount int64     `gorm:"column:view_count;type:bigint"`
	CreatedAt time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
}

func (IMDBYoutube) TableName() string {
	return "imdb_youtube"
}

type IMDBVideo struct {
	Id        uint64    `gorm:"column:id;type:bigint;primary_key"`
	FilmId    string    `gorm:"column:film_id;type:varchar(20)"`
	PlayUrl   string    `gorm:"column:play_url;type:varchar(100)"`
	Cover     string    `gorm:"column:cover;type:varchar(300)"`
	Duration  int32     `gorm:"column:duration;type:int"`
	CreatedAt time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
}

func (IMDBVideo) TableName() string {
	return "imdb_video"
}

type YoutubeItem struct {
	Name      string `json:"name"`
	PlayUrl   string `json:"play_url"`
	Cover     string `json:"cover"`
	Duration  int32  `json:"duration"`
	ViewCount int64  `json:"view_count"`
}

type VideoItem struct {
	PlayUrl   string `json:"play_url"`
	Cover     string `json:"cover"`
	Duration  int32  `json:"duration"`
}

func QueryTorrent(ctx context.Context, filmId string) (infohashs []string, err error) {
	infohashs = make([]string, 0, 10)

	var records []IMDBTorrent
	err = dbConn.Where("film_id = ?", filmId).Find(ctx, &records).Error
	if err != nil {
		return
	}

	for _, record := range records {
		infohashs = append(infohashs, record.InfoHash)
	}
	return
}

func QueryYoutube(ctx context.Context, filmIds []string) (youtubeMap map[string][]YoutubeItem, err error) {
	youtubeMap = make(map[string][]YoutubeItem)
	if filmIds == nil || len(filmIds) == 0 {
		return
	}

	var records []IMDBYoutube
	err = dbConn.Where("film_id in (?)", filmIds).Find(ctx, &records).Error
	if err != nil {
		return
	}

	for _, record := range records {
		videos, ok := youtubeMap[record.FilmId]
		if !ok {
			videos = make([]YoutubeItem, 0, 5)
		}

		videos = append(videos, YoutubeItem{
			Name: record.VideoName,
			PlayUrl: fmt.Sprintf("https://m.youtube.com/watch?v=%s", record.PlayId),
			Cover: record.Cover,
			Duration: record.Duration,
			ViewCount: record.ViewCount,
		})
		youtubeMap[record.FilmId] = videos
	}
	return
}

func QueryVideo(ctx context.Context, filmIds []string) (videoMap map[string][]VideoItem, err error) {
	videoMap = make(map[string][]VideoItem)
	if filmIds == nil || len(filmIds) == 0 {
		return
	}

	var records []IMDBVideo
	err = dbConn.Where("film_id in (?)", filmIds).Find(ctx, &records).Error
	if err != nil {
		return
	}

	for _, record := range records {
		videos, ok := videoMap[record.FilmId]
		if !ok {
			videos = make([]VideoItem, 0, 10)
		}

		videos = append(videos, VideoItem{
			PlayUrl: record.PlayUrl,
			Cover: record.Cover,
			Duration: record.Duration,
		})
		videoMap[record.FilmId] = videos
	}
	return
}

func QueryTorrentUrl(ctx context.Context, infohashs []string) (downloadMap map[string]string, err error) {
	downloadMap = make(map[string]string)
	if infohashs == nil || len(infohashs) == 0 {
		return
	}

	var records []TorrentDownload
	err = dbConn.Where("infohash in (?)", infohashs).Find(ctx, &records).Error
	if err != nil {
		return
	}

	for _, record := range records {
		if record.Kind == 1 {
			downloadMap[record.InfoHash] = record.DownloadUrl
		}
	}
	return
}

func QuerySongUrl(ctx context.Context, songId []string) (downloadMap map[string]map[string]string, err error) {
	downloadMap = make(map[string]map[string]string)
	if songId == nil || len(songId) == 0 {
		return
	}

	var records []SongDownload
	err = dbConn.Where("song_id in (?)", songId).Find(ctx, &records).Error
	if err != nil {
		return
	}

	for _, record := range records {
		signUrl, e := signer.Sign(record.DownloadUrl, time.Now().Add(time.Hour * 1))
		if e != nil {
			continue
		}

		bitMap, ok := downloadMap[record.SongId]
		if ok {
			bitMap[record.BitRate] = signUrl
			downloadMap[record.SongId] = bitMap
		} else {
			bitMap := make(map[string]string)
			bitMap[record.BitRate] = signUrl
			downloadMap[record.SongId] = bitMap
		}
	}
	return
}

func QueryCollections(ctx context.Context, collectionIds []string) (collections []SongCollection, err error) {
	if collectionIds == nil || len(collectionIds) == 0 {
		collections = []SongCollection{}
		return
	}

	err = dbConn.Where("id in (?)", collectionIds).Find(ctx, &collections).Error
	return
}
