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
	router.GET("/yoyo/search", server.SearchBT)
	router.POST("/yoyo/update", server.UpdateBTMetaInfo)
	router.GET("/yoyo/movie/search", server.SearchMovie)
	router.GET("/yoyo/mv/search", server.SearchMV)
	router.GET("/yoyo/movie/discover", server.DiscoverMovie)
	router.GET("/yoyo/resource", server.GetResource)
	router.GET("/yoyo/resources", server.GetResources)
	router.POST("/yoyo/torrent", server.UploadTorrent)

	router.GET("/yoyo/song/search", server.SearchSong)
	router.GET("/yoyo/album/search", server.SearchAlbum)
	router.GET("/yoyo/album/discover", server.DiscoverAlbum)
	router.GET("/yoyo/singer/search", server.SearchSinger)
	router.GET("/yoyo/singer/hot", server.HotSinger)
	router.GET("/yoyo/album", server.GetAlbum)
	router.GET("/yoyo/albums", server.GetAlbums)
	router.GET("/yoyo/songs", server.GetSongs)
	router.GET("/yoyo/collection", server.GetCollection)
	router.GET("/yoyo/collections", server.GetCollections)
	router.GET("/yoyo/song/play", server.GetSongUrl)
	router.GET("/yoyo/all/search", server.SearchAll)

	endless.ListenAndServe(address, router)
	os.Exit(0)
}
