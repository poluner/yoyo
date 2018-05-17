package server

import (
	"fmt"
	"time"
	"context"
	"gopkg.in/redis.v5"
	"github.com/LiuRoy/xgorm"
)

var (
	dbConn *xgorm.DB
	redisConn *redis.Client
)


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

type TorrentItem struct {
	Name     string `json:"name"`
	InfoHash string `json:"infohash"`
	Length   int64  `json:"length"`
}

type YoutubeItem struct {
	Name      string `json:"name"`
	PlayUrl   string `json:"play_url"`
	Cover     string `json:"cover"`
	Duration  int32  `json:"duration"`
	ViewCount int64  `json:"view_count"`
}

func QueryTorrent(ctx context.Context, filmIds []string) (btMap map[string][]*TorrentItem, err error) {
	btMap = make(map[string][]*TorrentItem)
	if filmIds == nil || len(filmIds) == 0 {
		return
	}

	var records []IMDBTorrent
	err = dbConn.Where("film_id in (?)", filmIds).Find(ctx, &records).Error
	if err != nil {
		return
	}

	for _, record := range records {
		torrents, ok := btMap[record.FilmId]
		if !ok {
			torrents = make([]*TorrentItem, 0, 5)
		}

		torrents = append(torrents, &TorrentItem{
			Name: record.BtName,
			InfoHash: record.InfoHash,
			Length: record.Length,
		})
		btMap[record.FilmId] = torrents
	}
	return
}

func QueryYoutube(ctx context.Context, filmIds []string) (youtubeMap map[string][]*YoutubeItem, err error) {
	youtubeMap = make(map[string][]*YoutubeItem)
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
			videos = make([]*YoutubeItem, 0, 5)
		}

		videos = append(videos, &YoutubeItem{
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
