package server

const (
	ProjectName = "yoyo"

	esIndex = "torrent"
	esType  = "doc"

	noError       = 0
	internalErr   = 100
	paramsInvalid = 200
)

var (
	EsUrls = []string{"http://120.132.120.59:9200", "http://172.31.23.5:9201", "http://172.31.10.234:9200"}
)
