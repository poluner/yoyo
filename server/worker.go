package server

import (
	"bytes"
	"strings"
	"net/http"
	log "github.com/alecthomas/log4go"
)


var (
	downloadChannel = make(chan string, 1000)

	searchSingerChannel = make(chan *searchSingerRequest, 100)
	searchSongChannel   = make(chan *searchSongRequest, 100)
	searchAlbumChannel  = make(chan *searchAlbumRequest, 100)
	searchMovieChannel  = make(chan *searchMovieRequest, 100)
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

func SearchSingerWorker() {
	for {
		request := <-searchSingerChannel

		total, singers, err := request.Param.SearchSinger()
		if err != nil || total == 0 {
			request.singerChannel <- nil
			continue
		}

		singer := singers[0]
		input := strings.TrimSpace(request.Param.Text)
		if strings.ToLower(input) ==  strings.ToLower(singer.Title) {
			request.singerChannel <- singer
		}
	}
}

func SearchSongWorker() {
	for {
		request := <-searchSongChannel

		total, songs, maxScore, err := request.Param.SearchSong()
		if err != nil {
			request.songChannel <- nil
			continue
		}

		result := songResult{
			Total: total,
			MaxScore: maxScore,
			Data: songs,
		}
		request.songChannel <- &result
	}
}

func SearchAlbumWorker() {
	for {
		request := <-searchAlbumChannel

		total, albums, maxScore, err := request.Param.SearchAlbum()
		if err != nil {
			request.albumChannel <- nil
			continue
		}

		result := albumResult{
			Total: total,
			MaxScore: maxScore,
			Data: albums,
		}
		request.albumChannel <- &result
	}
}

func SearchMovieWorker() {
	for {
		request := <-searchMovieChannel

		total, movies, maxScore, err := request.Param.SearchMovie()
		if err != nil {
			request.movieChannel <- nil
			continue
		}

		result := movieResult{
			Total: total,
			MaxScore: maxScore,
			Data: movies,
		}
		request.movieChannel <- &result
	}
}
