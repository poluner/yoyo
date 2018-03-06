package main

import (
	"fmt"
	"github.com/anacrolix/torrent/metainfo"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
	"github.com/olivere/elastic"
	"golang.org/x/net/context"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"time"
)

var (
	dbConnection *gorm.DB
	esClient     *elastic.Client
	getTorrent   string
)

type File struct {
	Path   []string `json:"path"`
	Length int64    `json:"length"`
}

type BitTorrent struct {
	Name        string    `json:"name"`
	Name2       string    `json:"name2"`
	Files       []File    `json:"files,omitempty"`
	Length      int64     `json:"length,omitempty"`
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
	var err error
	dbConnection, err = gorm.Open("mysql", "watchnow:watchnow2018@tcp(172.31.21.32:3306)/yoyo?charset=utf8mb4&parseTime=True&loc=Local")
	if err != nil {
		panic(err)
	}

	esClient, err = elastic.NewClient(elastic.SetURL("http://172.31.23.5:9200", "http://172.31.23.5:9201", "http://172.31.10.234:9200"))
	if err != nil {
		panic(err)
	}

	currentFile, err := exec.LookPath(os.Args[0])
	if err != nil {
		panic(err)
	}
	absPath, err := filepath.Abs(currentFile)
	if err != nil {
		panic(err)
	}
	currentDir, _ := path.Split(absPath)
	getTorrent = path.Join(currentDir, "gettorrent")
}

func downloadTorrent(infohash string) (success bool, err error) {
	cmd := exec.Command(getTorrent, infohash)
	out, err := cmd.Output()
	if err != nil {
		return
	}

	if string(out) == "yes" {
		success = true
	}
	return
}

func retrieveMetaData(infohash string) (bt *BitTorrent, err error) {
	torrentPath := fmt.Sprintf("/tmp/torrent/%s.torrent", infohash)
	mi, err := metainfo.LoadFromFile(torrentPath)
	if err != nil {
		return
	}

	info, err := mi.UnmarshalInfo()
	if err != nil {
		return
	}

	bt = &BitTorrent{
		Name:        info.Name,
		Name2:       info.Name,
		Length:      info.Length,
		CollectedAt: time.Now(),
	}
	var total int64
	for _, file := range info.Files {
		total += file.Length
		bt.Files = append(bt.Files, File{
			Length: file.Length,
			Path:   file.Path,
		})
	}
	if total > 0 {
		bt.Length = total
	}
	return
}

func main() {
	var records []Infohash
	for {
		findResult := dbConnection.Where("status = ?", 0).Limit(100).Find(&records)
		if findResult.RecordNotFound() {
			break
		}

		for _, record := range records {
			success, err := downloadTorrent(record.Infohash)
			if err != nil {
				continue
			}
			fmt.Println(record.Infohash, success)

			if !success {
				dbConnection.Table("infohash_task").Where(
					"infohash = ?", record.Infohash).UpdateColumn("status", 2)
				continue
			}

			bt, err := retrieveMetaData(record.Infohash)
			if err != nil {
				continue
			}

			_, err = esClient.Index().Index("torrent").Type(
				"doc").Id(record.Infohash).BodyJson(bt).Do(context.Background())
			if err != nil {
				continue
			}
			dbConnection.Table("infohash_task").Where(
				"infohash = ?", record.Infohash).UpdateColumn("status", 1)
		}
	}
}
