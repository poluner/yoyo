package server

import (
	"encoding/json"
	"github.com/gin-gonic/gin"
	"net/http"
	"strconv"
	"time"
	"context"
	"fmt"
)

type suggestParam struct {
	Text string `form:"text" binding:"required"`
	Size int    `form:"size"`
	Type string `form:"type"`

	ctx  context.Context
}

type searchParam struct {
	Text        string `form:"text"`
	Singer      string `form:"singer"`   // 歌手搜索时使用
	Offset      int    `form:"offset"`
	Limit       int    `form:"limit"`
	IgnoreFiles int    `form:"ignore_files"`

	ctx    context.Context
}

type metaInfo struct {
	Name   string     `json:"name"`
	Length int        `json:"length"`
	Files  []FileItem `json:"files,omitempty"`
}

type updateParam struct {
	Meta     metaInfo `json:"info"`
	Hot      int      `json:"hot"`
	Infohash string   `json:"infohash"`

	ctx      context.Context
}

type discoverParam struct {
	Genre    string    `form:"genre"`
	Year     int       `form:"year"`
	Country  string    `form:"country"`
	Language string    `form:"language"`
	Sort     string    `form:"sort"`
	Ascend   int       `form:"ascending"`

	Offset   int       `form:"offset"`
	Limit    int       `form:"limit"`

	ctx      context.Context
}

type getParam struct {
	Id       string    `form:"id" binding:"required"`
	ctx      context.Context
}

type mgetParam struct {
	Ids      []string  `form:"id" binding:"required"`
	ctx      context.Context
}


func Suggest(c *gin.Context) {
	var (
		err   error
		param suggestParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	if param.Size == 0 {
		param.Size = 10
	}
	if param.Type == "" {
		param.Type = "torrent"
	}
	param.ctx = c.Request.Context()

	result, err := param.Suggest()
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "search failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"data":   result,
	})
}

func SearchBT(c *gin.Context) {
	var (
		err   error
		param searchParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	if param.Limit == 0 {
		param.Limit = 10
	}
	param.ctx = c.Request.Context()
	total, result, err := param.SearchBT()

	// kinesis 监控
	event := KEvent{
		EventClass: 1,
		EventName:  "bt_search",
		Attributes: []string{"search", "torrent"},
		ExtData: map[string]string{
			"text":    param.Text,
			"offset":  strconv.Itoa(param.Offset),
			"limit":   strconv.Itoa(param.Limit),
			"total":   strconv.Itoa(int(total)),
			"length":  strconv.Itoa(len(result)),
			"version": "1.0",
		},
		RequestHeader: c.Request.Header,
		CreateTime:    time.Now(),
	}
	event.Push(c.Request.Context())

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "search failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"total":  total,
		"data":   result,
	})
}

func UpdateBTMetaInfo(c *gin.Context) {
	var (
		param updateParam
		err error
	)
	decoder := json.NewDecoder(c.Request.Body)
	if err = decoder.Decode(&param); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": err,
			"code":   paramsInvalid,
		})
		return
	}

	param.ctx = c.Request.Context()
	downloadChannel <- param.Infohash

	err = param.UpdateTorrent()
	if err != nil {
		c.JSON(http.StatusOK, gin.H{
			"result": err,
			"code":   paramsInvalid,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
	})
}

func SearchMovie(c *gin.Context) {
	var (
		err   error
		param searchParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	if param.Limit == 0 {
		param.Limit = 10
	}
	param.ctx = c.Request.Context()
	total, result, err := param.SearchMovie()

	event := KEvent{
		EventClass: 1,
		EventName:  "movie_search",
		Attributes: []string{"search", "movie"},
		ExtData: map[string]string{
			"text":    param.Text,
			"offset":  strconv.Itoa(param.Offset),
			"limit":   strconv.Itoa(param.Limit),
			"total":   strconv.Itoa(int(total)),
			"length":  strconv.Itoa(len(result)),
			"version": "1.0",
		},
		RequestHeader: c.Request.Header,
		CreateTime:    time.Now(),
	}
	event.Push(c.Request.Context())

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "search failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"total":  total,
		"data":   result,
	})
}

func SearchMV(c *gin.Context) {
	var (
		err   error
		param searchParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	if param.Limit == 0 {
		param.Limit = 10
	}
	param.ctx = c.Request.Context()
	total, result, err := param.SearchMV()

	event := KEvent{
		EventClass: 1,
		EventName:  "mv_search",
		Attributes: []string{"search", "mv"},
		ExtData: map[string]string{
			"text":    param.Text,
			"offset":  strconv.Itoa(param.Offset),
			"limit":   strconv.Itoa(param.Limit),
			"total":   strconv.Itoa(int(total)),
			"length":  strconv.Itoa(len(result)),
			"version": "1.0",
		},
		RequestHeader: c.Request.Header,
		CreateTime:    time.Now(),
	}
	event.Push(c.Request.Context())

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "search failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"total":  total,
		"data":   result,
	})
}

func DiscoverMovie(c *gin.Context) {
	var (
		err   error
		param discoverParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	if param.Limit == 0 {
		param.Limit = 10
	}
	if param.Sort == "" {
		param.Sort = "rating_value"
	}
	param.ctx = c.Request.Context()
	total, result, err := param.DiscoverMovie()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "search failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"total":  total,
		"data":   result,
	})
}

func GetResource(c *gin.Context) {
	var (
		err   error
		param getParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	param.ctx = c.Request.Context()
	result, err := param.GetResource()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "get failed",
			"code":   internalErr,
		})
		return
	}

	if result == nil {
		c.JSON(http.StatusOK, gin.H{
			"result": "no resource",
			"code":   noError,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"data":   result,
	})
}

func GetResources(c *gin.Context) {
	var (
		err   error
		param mgetParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	param.ctx = c.Request.Context()
	result, err := param.GetResources()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "get failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"data":   result,
	})
}

func UploadTorrent(c *gin.Context) {
	ctx := c.Request.Context()
	infohash := c.PostForm("infohash")
	if infohash != "" {
		var record TorrentDownload
		if !dbConn.Where("infohash = ?", infohash).First(
			ctx, &record).RecordNotFound() {
			c.JSON(http.StatusOK, gin.H{
				"result": "torrent exist",
				"code":   noError,
			})
			return
		}
	} else {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "infohash empty",
			"code": paramsInvalid,
		})
		return
	}

	torrent, err := c.FormFile("torrent")
	if err != nil {
		c.JSON(http.StatusForbidden, gin.H{
			"result": "no torrent file",
			"code": UploadErr,
		})
		return
	}

	torrentFile, err := torrent.Open()
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "open torrent error",
			"code": torrent,
		})
		return
	}

	data := make([]byte, 1048576)
	n, err := torrentFile.Read(data)
	if n >= int(1048576) {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "torrent size too large",
			"code": UploadErr,
		})
		return
	}

	err = Upload(ctx, infohash, data[:n])
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "upload s3 failed",
			"code": UploadErr,
		})
		return
	}

	record := TorrentDownload{
		InfoHash: infohash,
		DownloadUrl: fmt.Sprintf("http://d1saub1vswkfmx.cloudfront.net/%s.torrent", infohash),
		Kind: 1,
	}
	dbConn.Create(ctx, &record)
	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code": noError,
	})
}

func SearchSong(c *gin.Context) {
	var (
		err   error
		param searchParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	if param.Limit == 0 {
		param.Limit = 10
	}
	param.ctx = c.Request.Context()
	total, result, err := param.SearchSong()

	event := KEvent{
		EventClass: 1,
		EventName:  "song_search",
		Attributes: []string{"search", "song"},
		ExtData: map[string]string{
			"text":    param.Text,
			"offset":  strconv.Itoa(param.Offset),
			"limit":   strconv.Itoa(param.Limit),
			"total":   strconv.Itoa(int(total)),
			"length":  strconv.Itoa(len(result)),
			"version": "1.0",
		},
		RequestHeader: c.Request.Header,
		CreateTime:    time.Now(),
	}
	event.Push(c.Request.Context())

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "search failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"total":  total,
		"data":   result,
	})
}

func SearchAlbum(c *gin.Context) {
	var (
		err   error
		param searchParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	if param.Limit == 0 {
		param.Limit = 10
	}
	param.ctx = c.Request.Context()
	total, result, err := param.SearchAlbum()

	event := KEvent{
		EventClass: 1,
		EventName:  "album_search",
		Attributes: []string{"search", "album"},
		ExtData: map[string]string{
			"text":    param.Text,
			"offset":  strconv.Itoa(param.Offset),
			"limit":   strconv.Itoa(param.Limit),
			"total":   strconv.Itoa(int(total)),
			"length":  strconv.Itoa(len(result)),
			"version": "1.0",
		},
		RequestHeader: c.Request.Header,
		CreateTime:    time.Now(),
	}
	event.Push(c.Request.Context())

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "search failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"total":  total,
		"data":   result,
	})
}

func DiscoverAlbum(c *gin.Context) {
	var (
		err   error
		param discoverParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	if param.Limit == 0 {
		param.Limit = 10
	}
	param.ctx = c.Request.Context()
	total, result, err := param.DiscoverAlbum()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "search failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"total":  total,
		"data":   result,
	})
}

func GetAlbum(c *gin.Context) {
	var (
		err   error
		param getParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	param.ctx = c.Request.Context()
	result, err := param.GetAlbum()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "get failed",
			"code":   internalErr,
		})
		return
	}

	if result == nil {
		c.JSON(http.StatusOK, gin.H{
			"result": "no resource",
			"code":   noError,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"data":   result,
	})
}

func GetAlbums(c *gin.Context) {
	var (
		err   error
		param mgetParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	param.ctx = c.Request.Context()
	result, err := param.GetAlbums()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "get failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"data":   result,
	})
}

func GetSongs(c *gin.Context) {
	var (
		err   error
		param mgetParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	param.ctx = c.Request.Context()
	result, err := param.GetSongs()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "get failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"data":   result,
	})
}

func GetCollection(c *gin.Context) {
	var (
		err   error
		param getParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	param.ctx = c.Request.Context()
	result, err := param.GetCollection()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "get failed",
			"code":   internalErr,
		})
		return
	}

	if result == nil {
		c.JSON(http.StatusOK, gin.H{
			"result": "no resource",
			"code":   noError,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"data":   result,
	})
}

func GetCollections(c *gin.Context) {
	var (
		err   error
		param mgetParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	param.ctx = c.Request.Context()
	result, err := param.GetCollections()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "get failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"data":   result,
	})
}

func GetSongUrl(c *gin.Context) {
	var (
		err   error
		param getParam
	)

	err = c.BindQuery(&param)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": "params invalid",
			"code":   paramsInvalid,
		})
		return
	}

	param.ctx = c.Request.Context()
	result, err := param.GetSongUrl()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"result": "get failed",
			"code":   internalErr,
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
		"data":   result,
	})
}
