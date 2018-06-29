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
	Channel       chan *Singer
}

type searchSongRequest struct {
	Param         searchParam
	Channel       chan *songResult
}

type searchAlbumRequest struct {
	Param         searchParam
	Channel   chan *albumResult
}

type searchMovieRequest struct {
	Param         searchParam
	Channel   chan *movieResult
}

type searchAllResult struct {
	Movie  *movieResult  `json:"movie"`
	Album  *albumResult  `json:"album"`
	Song   *songResult   `json:"song"`
	Singer *Singer       `json:"singer"`
}

func (p *searchParam) SearchAll() (result *searchAllResult, err error) {
	singerRequest := searchSingerRequest{
		Param: *p,
		Channel: make(chan *Singer, 1),
	}
	searchSingerChannel <- &singerRequest

	songRequest := searchSongRequest{
		Param: *p,
		Channel: make(chan *songResult, 1),
	}
	searchSongChannel <- &songRequest

	albumRequest := searchAlbumRequest{
		Param: *p,
		Channel: make(chan *albumResult, 1),
	}
	searchAlbumChannel <- &albumRequest

	movieRequest := searchMovieRequest{
		Param: *p,
		Channel: make(chan *movieResult, 1),
	}
	searchMovieChannel <- &movieRequest

	result = &searchAllResult{}
	for i := 0; i < 4; i++ {
		select {
		case singer := <-singerRequest.Channel:
			result.Singer = singer
		case song := <-songRequest.Channel:
			result.Song = song
		case album := <-albumRequest.Channel:
			result.Album = album
		case movie := <-movieRequest.Channel:
			result.Movie = movie
		case <-time.After(time.Second * 1):
			log.Info("search time out")
			err = errors.New("search all timeout")
			return
		}
	}
	return
}
