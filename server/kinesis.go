package server

import (
	"context"
	"encoding/json"
	log "github.com/alecthomas/log4go"
	"github.com/aws/aws-sdk-go/aws"
	"github.com/aws/aws-sdk-go/service/kinesis"
	"github.com/satori/go.uuid"
	"net/http"
	"time"
)

type KEvent struct {
	EventClass    int8              `json:"event_class"`
	EventName     string            `json:"event_name"`
	Attributes    []string          `json:"attributes"`
	ExtData       map[string]string `json:"ext_data"`
	RequestHeader http.Header       `json:"request_header"`
	CreateTime    time.Time         `json:"create_time"`
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

	result, err := svc.PutRecordWithContext(ctx, &kinesis.PutRecordInput{
		Data:         bytePostData,
		StreamName:   aws.String(kinesisStream),
		PartitionKey: aws.String(id.String()),
	})
	log.Info("kinesis: %+v, %v", result, err)
	return
}
