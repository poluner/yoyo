package server

import (
	"time"
	"context"
	"github.com/LiuRoy/xgorm"
)

var dbConn *xgorm.DB


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
	VideoName string    `gorm:"column:infohash;type:varchar(300)"`
	PlayUrl   string    `gorm:"column:bt_name;type:varchar(100)"`
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
	Name     string `json:"name"`
	PlayUrl  string `json:"play_url"`
}

func QueryTorrent(ctx context.Context, filmIds []string) (btMap map[string][]TorrentItem, err error) {
	btMap = make(map[string][]TorrentItem)
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
			torrents = make([]TorrentItem, 0, 5)
		}

		torrents = append(torrents, TorrentItem{
			Name: record.BtName,
			InfoHash: record.InfoHash,
			Length: record.Length,
		})
		btMap[record.FilmId] = torrents
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
		torrents, ok := youtubeMap[record.FilmId]
		if !ok {
			torrents = make([]YoutubeItem, 0, 5)
		}

		torrents = append(torrents, YoutubeItem{
			Name: record.VideoName,
			PlayUrl: record.PlayUrl,
		})
		youtubeMap[record.FilmId] = torrents
	}
	return
}
