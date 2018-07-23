package server

const (
	ProjectName = "yoyo"
	samplePath  = "sample.json"

	resourceIndex = "resources"
	resourceType  = "doc"

	songIndex = "songs"
	songType  = "doc"

	suggestIndex = "suggest"
	suggestType  = "doc"

	whatsAppRedisList = "whatsapp_status_list"

	noError       = 0
	internalErr   = 100
	paramsInvalid = 200
	UploadErr     = 300

	maxResultWindow = 10000
)

var (
	crackedSite = map[string]bool{
		"youtube": true,
	}

	s3bucket          = "torrenthub"
	hotSingerIds      = []string{
		"singer7786",
		"singer110",
		"singer119593",
		"singer4",
		"singer52767",
		"singer835589",
		"singer36",
		"singer54397",
		"singer73",
		"singer159714",
		"singer758767",
		"singer495",
		"singer1377509",
		"singer155",
		"singer362437",
		"singer696324",
		"singer209438",
		"singer252190",
		"singer119850",
		"singer43022",
		"singer7197",
		"singer113346",
		"singer42",
		"singer243108",
		"singer154",
		"singer123822",
		"singer125941",
		"singer200459",
		"singer12",
		"singer55",
		"singer13074",
		"singer115",
		"singer214612",
		"singer119",
		"singer13",
		"singer712110",
		"singer172144",
		"singer137921",
		"singer53579",
		"singer566",
		"singer1",
		"singer37",
	}
)
