package server

const (
	ProjectName = "yoyo"

	esIndex = "resources"
	esType  = "doc"

	noError       = 0
	internalErr   = 100
	paramsInvalid = 200

	xrayDaemonAddress = "127.0.0.1:3000"
	kinesisRegion     = "ap-south-1"
	kinesisStream     = "SeverReportProduct"
)

var (
	EsUrls = []string{"https://search-xles03-xw27kvqlra4onvljuqbbemmb4q.ap-south-1.es.amazonaws.com"}
)
