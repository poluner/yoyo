package server

import (
	"context"
	"encoding/json"
	"github.com/aws/aws-sdk-go/aws"
	"github.com/aws/aws-sdk-go/aws/session"
	"github.com/aws/aws-sdk-go/service/kinesis"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/satori/go.uuid"
	"time"
)

var (
	svc *kinesis.Kinesis
)

func init() {
	ses, err := session.NewSession(&aws.Config{
		Region: aws.String(kinesisRegion),
	})
	if err != nil {
		panic(err)
	}

	svc = kinesis.New(ses)
	xray.AWS(svc.Client)
}

type KEvent struct {
	EventClass    int8                   `json:"event_class"`
	EventName     string                 `json:"event_name"`
	Attributes    []string               `json:"attributes"`
	ExtData       map[string]interface{} `json:"ext_data"`
	RequestHeader interface{}            `json:"request_header"`
	CreateTime    time.Time              `json:"create_time"`
}

func (e *KEvent) Push(ctx context.Context) (err error) {
	bytePostData, err := json.Marshal(e)
	if err != nil {
		return
	}

	id, err := uuid.NewV4()
	if err != nil {
		return
	}

	_, err = svc.PutRecordWithContext(ctx, &kinesis.PutRecordInput{
		Data:         bytePostData,
		StreamName:   aws.String(kinesisStream),
		PartitionKey: aws.String(id.String()),
	})
	return
}
