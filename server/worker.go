package server

import (
	"bytes"
	"net/http"
	log "github.com/alecthomas/log4go"
)


var (
	downloadChannel = make(chan string, 1000)
)

type downloadParam struct {
	Infohash string `json:"infohash"`
}


func DownloadTorrentWorker() {
	for {
		infohash := <- downloadChannel
		log.Info("infohash: %s", infohash)

		payload := downloadParam{Infohash: infohash}
		body, err := json.Marshal(&payload)
		if err != nil {
			log.Error(err)
		}

		res, err := http.Post(downloadUrl, "application/json", bytes.NewBuffer(body))
		if err != nil {
			log.Error(err)
		}

		if res != nil {
			res.Body.Close()
		}
	}
}
