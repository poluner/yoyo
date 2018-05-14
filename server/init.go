package server

import (
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
)


func init() {
	prometheus.MustRegister(ResponseCounter)
	prometheus.MustRegister(ErrorCounter)
	prometheus.MustRegister(ResponseLatency)

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
	dbConn.LogMode(true)

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

	svc = kinesis.New(ses)
	xray.AWS(svc.Client)
}
