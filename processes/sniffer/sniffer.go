package main

import (
	"encoding/hex"
	"github.com/LiuRoy/yoyo/dht"
	log "github.com/alecthomas/log4go"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
	"time"
)

var (
	dbConnection *gorm.DB
)

func init() {
	db, err := gorm.Open("mysql", "watchnow:watchnow2018@tcp(172.31.21.32:3306)/yoyo?charset=utf8mb4&parseTime=True&loc=Local")
	if err != nil {
		panic(err)
	}
	dbConnection = db

	log.LoadConfiguration("logging.xml")
}

type Infohash struct {
	Id        uint64    `gorm:"column:id;type:bigint;primary_key"`
	Infohash  string    `gorm:"column:infohash;type:varchar(40)"`
	Hot       uint64    `gorm:"column:hot;type:bigint"`
	Status    uint8     `gorm:"column:status;type:tinyint"`
	CreatedAt time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
	UpdatedAt time.Time `gorm:"column:updated_at" sql:"DEFAULT:current_timestamp"`
}

func (Infohash) TableName() string {
	return "infohash_task"
}

func Insert(infohash string) {
	var record Infohash

	tx := dbConnection.Begin()
	if tx.Where("infohash = ?", infohash).Find(&record).RecordNotFound() {
		record.Infohash = infohash
		if tx.Create(&record).Error != nil {
			tx.Rollback()
			log.Error("%s insert failed", infohash)
		} else {
			tx.Commit()
		}
	} else {
		status := record.Status
		if status == 2 {
			status = 3
		}

		if tx.Model(&record).Updates(Infohash{
			Hot: record.Hot + 1,
		}).Error != nil {
			tx.Rollback()
			log.Error("%s update failed", infohash)
		} else {
			tx.Commit()
		}
	}
}

func main() {
	config := dht.NewCrawlConfig()
	config.OnAnnouncePeer = func(infoHash, ip string, port int) {
		Insert(hex.EncodeToString([]byte(infoHash)))
	}
	config.OnGetPeers = func(infoHash, ip string, port int) {
		Insert(hex.EncodeToString([]byte(infoHash)))
	}
	d := dht.New(config)

	d.Run()
}
