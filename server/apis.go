package server

import (
	"github.com/gin-gonic/gin"
	"net/http"
)

type suggestParam struct {
	Text string `form:"text" binding:"required"`
	Size int    `form:"size"`
}

type searchParam struct {
	Text   string `form:"text" binding:"required"`
	Offset int    `form:"offset"`
	Limit  int    `form:"limit"`
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

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
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

	c.JSON(http.StatusOK, gin.H{
		"result": "ok",
		"code":   noError,
	})
}
