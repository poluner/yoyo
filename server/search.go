package server

import (
	"time"
	"errors"
	log "github.com/alecthomas/log4go"
)

type movieResult struct {
	Total    int64       `json:"total"`
	MaxScore float64     `json:"max_score"`
	Data     []*Resource `json:"data"`
}

type albumResult struct {
	Total    int64    `json:"total"`
	MaxScore float64  `json:"max_score"`
	Data     []*Album `json:"data"`
}

type songResult struct {
	Total    int64   `json:"total"`
	MaxScore float64 `json:"max_score"`
	Data     []*Song `json:"data"`
}

type searchSingerRequest struct {
	Param         searchParam
	singerChannel chan *Singer
}

type searchSongRequest struct {
	Param         searchParam
	songChannel   chan *songResult
}

type searchAlbumRequest struct {
	Param         searchParam
	albumChannel   chan *albumResult
}

type searchMovieRequest struct {
	Param         searchParam
	movieChannel   chan *movieResult
}

type searchAllResult struct {
	Movie  *movieResult  `json:"movie"`
	Album  *albumResult  `json:"album"`
	Song   *songResult   `json:"song"`
	Singer *Singer       `json:"singer"`
}

func (p *searchParam) SearchAll() (result *searchAllResult, err error) {
	singerRequest := &searchSingerRequest{Param: *p}
	searchSingerChannel <- singerRequest

	songRequest := &searchSongRequest{Param: *p}
	searchSongChannel <- songRequest

	albumRequest := &searchAlbumRequest{Param: *p}
	searchAlbumChannel <- albumRequest

	movieRequest := &searchMovieRequest{Param: *p}
	searchMovieChannel <- movieRequest

	result = &searchAllResult{}
	for i := 0; i < 4; i++ {
		select {
		case singer := <-singerRequest.singerChannel:
			result.Singer = singer
			log.Info("aaaaaa singer channel output data. data:%+v", singer)
		case song := <-songRequest.songChannel:
			result.Song = song
			log.Info("aaaaaa song channel output data. data:%+v", song)
		case album := <-albumRequest.albumChannel:
			result.Album = album
			log.Info("aaaaaa album channel output data. data:%+v", album)
		case movie := <-movieRequest.movieChannel:
			result.Movie = movie
			log.Info("aaaaaa movie channel output data. data:%+v", movie)
		case <-time.After(time.Second * 1):
			log.Info("aaaaaa time out")
			err = errors.New("search all timeout")
			return
		}
	}
	return
}
