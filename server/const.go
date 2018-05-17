package server

const (
	ProjectName = "yoyo"
	samplePath  = "sample.json"

	esIndex = "resources"
	esType  = "doc"

	noError       = 0
	internalErr   = 100
	paramsInvalid = 200

	xrayDaemonAddress = "127.0.0.1:3000"
	kinesisRegion     = "ap-south-1"
	kinesisStream     = "SeverReportProduct"

	maxResultWindow = 100000

	mysqlUrl = "xxxxx:xxxxx@tcp(10.1.1.154:3306)/link?charset=utf8mb4&parseTime=True&loc=Local"
	esUrl    = "http://xxxxxx.com"
	redisUrl = "redis://xxxxx"
)
