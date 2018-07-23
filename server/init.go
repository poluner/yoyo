package server

import (
	"regexp"
	"gopkg.in/redis.v5"
	"github.com/LiuRoy/xgorm"
	"github.com/olivere/elastic"
	"github.com/aws/aws-sdk-go/aws"
	"github.com/aws/aws-sdk-go/aws/session"
	"github.com/aws/aws-sdk-go/service/kinesis"
	_ "github.com/aws/aws-xray-sdk-go/plugins/beanstalk"
	_ "github.com/aws/aws-xray-sdk-go/plugins/ec2"
	_ "github.com/aws/aws-xray-sdk-go/plugins/ecs"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/prometheus/client_golang/prometheus"
	"github.com/aws/aws-xray-sdk-go/strategy/sampling"
	_ "github.com/LiuRoy/xgorm/dialects/mysql"
	"github.com/aws/aws-sdk-go/service/s3/s3manager"
	"github.com/aws/aws-sdk-go/service/cloudfront/sign"
	"github.com/json-iterator/go"
)

var (
	dbConn *xgorm.DB
	redisConn *redis.Client
	esClient *elastic.Client
	svc *kinesis.Kinesis
	uploader *s3manager.Uploader
	signer *sign.URLSigner

	musicImagePattern *regexp.Regexp

	instanceId  = "default"
	json        = jsoniter.ConfigCompatibleWithStandardLibrary
)


func init() {
	prometheus.MustRegister(ResponseCounter)
	prometheus.MustRegister(ErrorCounter)
	prometheus.MustRegister(ResponseLatency)

	musicImagePattern = regexp.MustCompile(`\d+x\d+`)

	var err error
	ss, err := sampling.NewLocalizedStrategyFromFilePath(samplePath)
	if err != nil {
		panic(err)
	}

	xray.Configure(xray.Config{
		DaemonAddr:     xrayDaemonAddress,
		LogLevel:       "info",
		SamplingStrategy: ss,
	})

	dbConn, err = xgorm.Open("mysql", mysqlUrl)
	if err != nil {
		panic(err)
	}
	dbConn.LogMode(false)

	redisOpt, err := redis.ParseURL(redisUrl)
	if err != nil {
		panic(err)
	}
	redisConn = redis.NewClient(redisOpt)

	esClient, err = elastic.NewSimpleClient(elastic.SetURL(esUrl), elastic.SetHttpClient(xray.Client(nil)))
	if err != nil {
		panic(err)
	}

	ses, err := session.NewSession(&aws.Config{
		Region: aws.String(kinesisRegion),
	})
	if err != nil {
		panic(err)
	}
	uploader = s3manager.NewUploader(ses)

	svc = kinesis.New(ses)
	xray.AWS(svc.Client)

	privateKey, err := sign.LoadPEMPrivKeyFile(s3Pri)
	if err != nil {
		panic(err)
	}
	signer = sign.NewURLSigner(s3Key, privateKey)

	// 获取机器的instance id
	instanceId, _ = getInstanceId()

	go SearchSingerWorker()
	go SearchSongWorker()
	go SearchMovieWorker()
	go SearchAlbumWorker()
}
