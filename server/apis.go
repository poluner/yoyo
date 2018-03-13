package server

import (
	"encoding/json"
	"github.com/gin-gonic/gin"
	"net/http"
)

type suggestParam struct {
	Text string `form:"text" binding:"required"`
	Size int    `form:"size"`
}

type searchParam struct {
	Text   string `form:"text"`
	Offset int    `form:"offset"`
	Limit  int    `form:"limit"`
}

type metaInfo struct {
	Name   string     `json:"name"`
	Length int        `json:"length"`
	Files  []FileItem `json:"files,omitempty"`
}

type updatePost struct {
	Meta metaInfo `json:"info"`
	Hot  int      `json:"hot"`
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

	result, err := EsSuggest(param.Text, param.Size)
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

func Search(c *gin.Context) {
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

	total, result, err := EsSearch(param.Text, param.Offset, param.Limit)
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

func UpdateDownloadCount(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
	})
}

func UpdateMetaInfo(c *gin.Context) {
	infohash := c.Param("infohash")

	var param updatePost
	var err error
	decoder := json.NewDecoder(c.Request.Body)
	if err = decoder.Decode(&param); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"result": err,
			"code":   paramsInvalid,
		})
		return
	}

	err = EsUpdateMetaData(infohash, &param)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
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
