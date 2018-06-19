package server

const (
	ProjectName = "yoyo"
	samplePath  = "sample.json"

	resourceIndex = "resources"
	resourceType  = "doc"

	songIndex = "song"
	songType  = "doc"

	noError       = 0
	internalErr   = 100
	paramsInvalid = 200
	UploadErr     = 300

	maxResultWindow = 10000
	downloadUrl = "http://api.watchnow.n0909.com/swallow/torrent"
)

var (
	crackedSite = map[string]bool{
		"youtube": true,
	}

	s3bucket          = "torrenthub"
)
