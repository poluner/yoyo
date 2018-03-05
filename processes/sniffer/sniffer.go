package main

import (
	"encoding/hex"
	"encoding/json"
	"fmt"
	"github.com/LiuRoy/yoyo/dht"
	log "github.com/alecthomas/log4go"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
	"github.com/olivere/elastic"
	"golang.org/x/net/context"
	"time"
)

var (
	dbConnection *gorm.DB
	wire         *dht.Wire
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

	wire = dht.NewWire(65536, 1024, 256)
	go func() {
		for resp := range wire.Response() {
			metadata, err := dht.Decode(resp.MetadataInfo)
			if err != nil {
				continue
			}
			info := metadata.(map[string]interface{})

			if _, ok := info["name"]; !ok {
				continue
			}

			infoHash := hex.EncodeToString(resp.InfoHash)
			bt := BitTorrent{
				Name: info["name"].(string),
			}
			bt.Name2 = bt.Name

			if v, ok := info["files"]; ok {
				files := v.([]interface{})
				bt.Files = make([]File, len(files))

				for i, item := range files {
					f := item.(map[string]interface{})
					bt.Files[i] = File{
						Path:   f["path"].([]interface{}),
						Length: f["length"].(int),
					}
				}
			} else if _, ok := info["length"]; ok {
				bt.Length = info["length"].(int)
			}

			insertIndex(infoHash, &bt)
			data, err := json.Marshal(bt)
			if err == nil {
				fmt.Printf("%s\n\n", data)
			}
		}
	}()
	go wire.Run()
}

func insertDatabase(infoStr string, isAnnounce bool) (finished bool) {
	var record Infohash
	infoHash := hex.EncodeToString([]byte(infoStr))
	tx := dbConnection.Begin()
	if tx.Where("infohash = ?", infoHash).Find(&record).RecordNotFound() {
		record.Infohash = infoHash
		if isAnnounce {
			record.AnnouncePeer += 1
		} else {
			record.GetPeer += 1
		}

		if tx.Create(&record).Error != nil {
			tx.Rollback()
			log.Error("%s insert failed", infoHash)
		} else {
			tx.Commit()
		}
	} else {
		var update Infohash
		if isAnnounce {
			update.AnnouncePeer = record.AnnouncePeer + 1
		} else {
			update.GetPeer = record.GetPeer + 1
		}

		if record.Status != 0 {
			finished = true
		}

		if tx.Model(&record).Updates(update).Error != nil {
			tx.Rollback()
			log.Error("%s update failed", infoHash)
		} else {
			tx.Commit()
		}
	}
	return
}

func insertIndex(infoHash string, bt *BitTorrent) {
	var total int
	for _, file := range bt.Files {
		total += file.Length
	}
	if total != 0 {
		bt.Length = total
	}
	bt.CollectedAt = time.Now()

	_, err := esClient.Index().Index("torrent").Type(
		"doc").Id(infoHash).BodyJson(bt).Do(context.Background())
	if err != nil {
		log.Error("%s insert es failed", infoHash)
		return
	}

	err = dbConnection.Table("infohash_task").Where(
		"infohash = ?", infoHash).UpdateColumn("status", 1).Error
	if err != nil {
		log.Error("%s update status to 1 failed", infoHash)
	}
}

func main() {
	config := dht.NewCrawlConfig()
	config.OnAnnouncePeer = func(infoHash, ip string, port int) {
		if !insertDatabase(infoHash, true) {
			wire.Request([]byte(infoHash), ip, port)
		}
	}
	config.OnGetPeers = func(infoHash, ip string, port int) {
		insertDatabase(infoHash, false)
	}
	d := dht.New(config)

	d.Run()
}
