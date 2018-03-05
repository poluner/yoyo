package server

const (
	ProjectName = "yoyo"

	esIndex = "torrent"
	esType  = "doc"

	noError       = 0
	internalErr   = 100
	paramsInvalid = 200

	SentryUrl = "http://9eace93dda3e4095b458210ff242af94:bf8112787c3e47d1ab87e9e24e74f985@tw06132.sandai.net:9000/137"
)

var (
	EsUrls = []string{"http://172.31.23.5:9200", "http://172.31.23.5:9201", "http://172.31.10.234:9200"}
)
