package main

import (
	"flag"
	"fmt"
	"os"
	"runtime"

	log "github.com/alecthomas/log4go"
	"github.com/fvbock/endless"
	"github.com/gin-gonic/gin"

	"github.com/LiuRoy/yoyo/server"
)

var (
	workerNum   int
	releaseMode bool
	address     string
)

func init() {
	cpuNum := runtime.NumCPU()
	flag.IntVar(&workerNum, "worker", cpuNum, "runtime MAXPROCS value")
	flag.BoolVar(&releaseMode, "release", false, "gin mode")
	flag.StringVar(&address, "address", "0.0.0.0:8197", "server address")

	log.LoadConfiguration("logging.xml")
}

func main() {
	flag.Parse()
	log.Info("address: %s MAXPROCS:%d release:%t", address, workerNum, releaseMode)
	runtime.GOMAXPROCS(workerNum)

	if releaseMode {
		gin.SetMode(gin.ReleaseMode)
	}
	router := gin.New()
	metricPath := fmt.Sprintf("/%s/metrics", server.ProjectName)
	router.Use(server.Metrics(metricPath))
	router.Use(server.Recovery())

	// 监控接口
	router.GET(metricPath, server.GetMetrics)

	router.GET("/yoyo/suggest", server.Suggest)
	router.GET("/yoyo/search", server.Search)
	router.POST("/yoyo/:infohash/update", server.UpdateMetaInfo)
	router.POST("/yoyo/:infohash/download", server.UpdateDownloadCount)

	endless.ListenAndServe(address, router)
	os.Exit(0)
}
