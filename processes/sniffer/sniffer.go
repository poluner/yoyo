package main

import (
	"bytes"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"github.com/LiuRoy/yoyo/dht"
	log "github.com/alecthomas/log4go"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
	"net/http"
	"time"
)

var (
	dbConnection *gorm.DB
	wire         *dht.Wire
	httpClient   *http.Client
)

type File struct {
	Path   []interface{} `json:"path"`
	Length int           `json:"length"`
}

type BitTorrent struct {
	Name   string `json:"name"`
	Files  []File `json:"files,omitempty"`
	Length int    `json:"length,omitempty"`
}

type updatePost struct {
	Meta BitTorrent `json:"info"`
	Hot  int        `json:"hot"`
}

type InfohashTask struct {
	Id        uint64    `gorm:"column:id;type:bigint;primary_key"`
	Infohash  string    `gorm:"column:infohash;type:varchar(40)"`
	Status    uint8     `gorm:"column:status;type:tinyint"`
	CreatedAt time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
	UpdatedAt time.Time `gorm:"column:updated_at" sql:"DEFAULT:current_timestamp"`
}

func (InfohashTask) TableName() string {
	return "infohash_task"
}

type AnnouncePeer struct {
	Id        uint64    `gorm:"column:id;type:bigint;primary_key"`
	Infohash  string    `gorm:"column:infohash;type:varchar(40)"`
	Address   string    `gorm:"column:address;type:varchar(20)"`
	Detail    string    `gorm:"column:detail;type:text"`
	CreatedAt time.Time `gorm:"column:created_at" sql:"DEFAULT:current_timestamp"`
}

func (AnnouncePeer) TableName() string {
	return "announce_peer"
}

func updateEs(infohash string, bt *updatePost) (err error) {
	postValue, err := json.Marshal(bt)
	if err != nil {
		return
	}

	url := fmt.Sprintf("http://api.watchnow.n0909.com/yoyo/%s/update", infohash)
	_, err = httpClient.Post(url, "application/json", bytes.NewBuffer(postValue))
	return
}

func init() {
	log.LoadConfiguration("logging.xml")
	httpClient = &http.Client{}

	var err error
	dbConnection, err = gorm.Open("mysql", "watchnow:watchnow@tcp(127.0.0.1:3307)/yoyo?charset=utf8mb4&parseTime=True&loc=Local")
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

			finishTask(infoHash, &bt)
			data, err := json.Marshal(bt)
			if err == nil {
				fmt.Printf("%s\n", infoHash)
				fmt.Printf("%s\n", data)
			}
		}
	}()
	go wire.Run()
}

func finishTask(infohash string, bt *BitTorrent) {
	if dbConnection.Table("infohash_task").Where(
		"infohash = ?", infohash).UpdateColumn("status", 1).Error != nil {
		log.Error("%s update status to 1 failed", infohash)
		return
	}

	var total int
	dbConnection.Table("announce_peer").Where("infohash = ?", infohash).Count(&total)
	if total > 0 {
		param := updatePost{
			Hot:  total,
			Meta: *bt,
		}
		updateEs(infohash, &param)
	}
}

func addGetPeerTask(infoStr string) {
	infoHash := hex.EncodeToString([]byte(infoStr))
	tx := dbConnection.Begin()
	var task InfohashTask
	if tx.Where("infohash = ?", infoHash).Find(&task).RecordNotFound() {
		task.Infohash = infoHash
		if tx.Create(&task).Error != nil {
			tx.Rollback()
			log.Error("addGetPeerTask %s insert infohash_task failed", infoHash)
			return
		}
	}
	tx.Commit()
	return
}

func addAnnouncePeerTask(infoStr string, address string) (finished bool) {
	infohash := hex.EncodeToString([]byte(infoStr))
	tx := dbConnection.Begin()
	var task InfohashTask
	if tx.Where("infohash = ?", infohash).Find(&task).RecordNotFound() {
		task.Infohash = infohash

		if tx.Create(&task).Error != nil {
			tx.Rollback()
			log.Error("addAnnnouncePeerTask %s insert infohash_task failed", infohash)
			return
		}
	} else {
		if task.Status != 0 {
			finished = true
		}
	}

	var announce AnnouncePeer
	var dupAddress bool
	if tx.Where("infohash = ? and address = ?", infohash,
		address).Find(&announce).RecordNotFound() {
		announce.Infohash = infohash
		announce.Address = address

		if tx.Create(&announce).Error != nil {
			tx.Rollback()
			log.Error("addAnnnouncePeerTask %s %s insert announce_peer failed", infohash)
			return
		}
	} else {
		dupAddress = true
	}
	tx.Commit()

	if task.Status == 1 && !dupAddress {
		var total int
		dbConnection.Table("announce_peer").Where("infohash = ?", infohash).Count(&total)

		if total > 0 {
			param := updatePost{
				Hot: total,
			}
			updateEs(infohash, &param)
		}
	}
	return
}

func main() {
	config := dht.NewCrawlConfig()
	config.OnAnnouncePeer = func(infoHash, ip string, port int) {
		address := fmt.Sprintf("%s:%d", ip, port)
		if !addAnnouncePeerTask(infoHash, address) {
			wire.Request([]byte(infoHash), ip, port)
		}
	}
	config.OnGetPeers = func(infoHash, ip string, port int) {
		addGetPeerTask(infoHash)
	}
	d := dht.New(config)

	d.Run()
}
