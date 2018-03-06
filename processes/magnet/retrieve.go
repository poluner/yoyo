package main

import (
	log "github.com/alecthomas/log4go"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
	"github.com/olivere/elastic"
	"golang.org/x/net/context"
	"time"
)

var (
	dbConnection *gorm.DB
	esClient     *elastic.Client
)

type File struct {
	Path   []interface{} `json:"path"`
	Length int           `json:"length"`
}

type BitTorrent struct {
	Name        string    `json:"name"`
	Name2       string    `json:"name2"`
	Files       []File    `json:"files,omitempty"`
	Length      int       `json:"length,omitempty"`
	CollectedAt time.Time `json:"collected_at"`
}

type Infohash struct {
	Id           uint64    `gorm:"column:id;type:bigint;primary_key"`
	Infohash     string    `gorm:"column:infohash;type:varchar(40)"`
	GetPeer      uint32    `gorm:"column:get_peer;type:int"`
	AnnouncePeer uint32    `gorm:"column:announce_peer;type:int"`
	Status       uint8     `gorm:"column:status;type:tinyint"`
	CreatedAt    time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
	UpdatedAt    time.Time `gorm:"column:updated_at" sql:"DEFAULT:current_timestamp"`
}

func (Infohash) TableName() string {
	return "infohash_task"
}

func init() {
	log.LoadConfiguration("logging.xml")

	var err error
	dbConnection, err = gorm.Open("mysql", "watchnow:watchnow2018@tcp(172.31.21.32:3306)/yoyo?charset=utf8mb4&parseTime=True&loc=Local")
	if err != nil {
		panic(err)
	}

	esClient, err = elastic.NewClient(elastic.SetURL("http://172.31.23.5:9200", "http://172.31.23.5:9201", "http://172.31.10.234:9200"))
	if err != nil {
		panic(err)
	}
}

func main() {
	var records []Infohash
	for {
		findResult := dbConnection.Where("status = ?", 0).Limit(100).Find(&records)
		if findResult.RecordNotFound() {
			break
		}

		for _, record := range records {

		}
	}
}
