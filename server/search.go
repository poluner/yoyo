package server

import (
	"time"
	"errors"
	"context"
	"github.com/aws/aws-xray-sdk-go/xray"
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
	_, seg := xray.BeginSubsegment(p.ctx, "all-search")
	defer seg.Close(err)

	singerRequest := &searchSingerRequest{
		Param: searchParam{
			Text: p.Text,
			Offset: p.Offset,
			Limit: p.Limit,
			ctx: context.Background(),
		},
	}
	searchSingerChannel <- singerRequest

	songRequest := &searchSongRequest{
		Param: searchParam{
			Text: p.Text,
			Offset: p.Offset,
			Limit: p.Limit,
			ctx: context.Background(),
		},
	}
	searchSongChannel <- songRequest

	albumRequest := &searchAlbumRequest{
		Param: searchParam{
			Text: p.Text,
			Offset: p.Offset,
			Limit: p.Limit,
			ctx: context.Background(),
		},
	}
	searchAlbumChannel <- albumRequest

	movieRequest := &searchMovieRequest{
		Param: searchParam{
			Text: p.Text,
			Offset: p.Offset,
			Limit: p.Limit,
			ctx: context.Background(),
		},
	}
	searchMovieChannel <- movieRequest

	result = &searchAllResult{}
	select {
	case singer := <-singerRequest.singerChannel:
		result.Singer = singer
	case song := <-songRequest.songChannel:
		result.Song = song
	case album := <-albumRequest.albumChannel:
		result.Album = album
	case movie := <-movieRequest.movieChannel:
		result.Movie = movie
	case <-time.After(time.Second * 1):
		err = errors.New("search all timeout")
		return
	}
	return
}
