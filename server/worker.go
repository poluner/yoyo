package server

import (
	"strings"
)


var (
	searchSingerChannel = make(chan *searchSingerRequest, 100)
	searchSongChannel   = make(chan *searchSongRequest, 100)
	searchAlbumChannel  = make(chan *searchAlbumRequest, 100)
	searchMovieChannel  = make(chan *searchMovieRequest, 100)
)

func SearchSingerWorker() {
	for {
		request := <-searchSingerChannel

		total, singers, err := request.Param.SearchSinger()
		if err != nil || total == 0 {
			request.Channel <- nil
			continue
		}

		singer := singers[0]
		input := strings.TrimSpace(request.Param.Text)
		if strings.ToLower(input) ==  strings.ToLower(singer.Title) {
			request.Channel <- singer
		} else {
			request.Channel <- nil
		}
	}
}

func SearchSongWorker() {
	for {
		request := <-searchSongChannel

		total, songs, maxScore, err := request.Param.SearchSong()
		if err != nil {
			request.Channel <- nil
			continue
		}

		result := songResult{
			Total: total,
			MaxScore: maxScore,
			Data: songs,
		}
		request.Channel <- &result
	}
}

func SearchAlbumWorker() {
	for {
		request := <-searchAlbumChannel

		total, albums, maxScore, err := request.Param.SearchAlbum()
		if err != nil {
			request.Channel <- nil
			continue
		}

		result := albumResult{
			Total: total,
			MaxScore: maxScore,
			Data: albums,
		}
		request.Channel <- &result
	}
}

func SearchMovieWorker() {
	for {
		request := <-searchMovieChannel

		total, movies, maxScore, err := request.Param.SearchMovie()
		if err != nil {
			request.Channel <- nil
			continue
		}

		result := movieResult{
			Total: total,
			MaxScore: maxScore / 5.0,
			Data: movies,
		}
		request.Channel <- &result
	}
}
