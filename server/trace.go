package server

import (
	"time"
	"bytes"
	"fmt"
	"strconv"
	"net/http"
	"github.com/gin-gonic/gin"
	log "github.com/alecthomas/log4go"
	"github.com/aws/aws-xray-sdk-go/header"
	"github.com/aws/aws-xray-sdk-go/xray"
	"github.com/prometheus/client_golang/prometheus"
	"github.com/prometheus/client_golang/prometheus/promhttp"
)

var (
	historyBuckets = [...]float64{
		10., 20., 30., 50., 80., 100., 200., 300., 500., 1000., 2000., 3000.}

	ResponseCounter = prometheus.NewCounterVec(prometheus.CounterOpts{
		Name: fmt.Sprintf("%s_requests_total", ProjectName),
		Help: "Total request counts"}, []string{"method", "endpoint", "instance"})
	ErrorCounter = prometheus.NewCounterVec(prometheus.CounterOpts{
		Name: fmt.Sprintf("%s_error_total", ProjectName),
		Help: "Total Error counts"}, []string{"method", "endpoint", "instance"})
	ResponseLatency = prometheus.NewHistogramVec(prometheus.HistogramOpts{
		Name:    fmt.Sprintf("%s_response_latency_millisecond", ProjectName),
		Help:    "Response latency (millisecond)",
		Buckets: historyBuckets[:]}, []string{"method", "endpoint", "instance"})

	sn = xray.NewFixedSegmentNamer(ProjectName)
)


func Metrics(notLogged ...string) gin.HandlerFunc {
	var skip map[string]struct{}

	if length := len(notLogged); length > 0 {
		skip = make(map[string]struct{}, length)

		for _, path := range notLogged {
			skip[path] = struct{}{}
		}
	}

	return func(c *gin.Context) {
		r := c.Request
		path := r.URL.Path
		if _, ok := skip[path]; ok {
			// Process request
			c.Next()
		} else {
			// Start timer
			start := time.Now()

			// xray http trace before operation
			name := sn.Name(c.Request.Host)
			traceHeader := header.FromString(r.Header.Get("x-amzn-trace-id"))
			ctx, seg := xray.NewSegmentFromHeader(r.Context(), name, traceHeader)
			r = r.WithContext(ctx)
			c.Request = r
			seg.Lock()

			scheme := "https://"
			if r.TLS == nil {
				scheme = "http://"
			}
			seg.GetHTTP().GetRequest().Method = r.Method
			seg.GetHTTP().GetRequest().URL = scheme + r.Host + r.URL.Path
			seg.GetHTTP().GetRequest().ClientIP, seg.GetHTTP().GetRequest().XForwardedFor = clientIP(r)
			seg.GetHTTP().GetRequest().UserAgent = r.UserAgent()

			var respHeader bytes.Buffer
			respHeader.WriteString("Root=")
			respHeader.WriteString(seg.TraceID)

			if traceHeader.SamplingDecision != header.Sampled && traceHeader.SamplingDecision != header.NotSampled {
				seg.Sampled = seg.ParentSegment.GetConfiguration().SamplingStrategy.ShouldTrace(r.Host, r.URL.Path, r.Method)
			}
			if traceHeader.SamplingDecision == header.Requested {
				respHeader.WriteString(";Sampled=")
				respHeader.WriteString(strconv.Itoa(btoi(seg.Sampled)))
			}

			c.Writer.Header().Set("x-amzn-trace-id", respHeader.String())
			seg.Unlock()

			// Process request
			c.Next()

			clientIP := c.ClientIP()
			method := c.Request.Method
			statusCode := c.Writer.Status()
			comment := c.Errors.ByType(gin.ErrorTypePrivate).String()

			seg.Lock()
			seg.GetHTTP().GetResponse().Status = c.Writer.Status()
			seg.GetHTTP().GetResponse().ContentLength, _ = strconv.Atoi(c.Writer.Header().Get("Content-Length"))

			if statusCode >= 400 && statusCode < 500 {
				seg.Error = true
			}
			if statusCode == 429 {
				seg.Throttle = true
			}
			if statusCode >= 500 && statusCode < 600 {
				seg.Fault = true
			}
			seg.Unlock()
			seg.Close(nil)

			// Stop timer
			end := time.Now()
			latency := end.Sub(start)

			log.Info("%3d | %13v | %-15s | %-7s %s %s",
				statusCode,
				latency,
				clientIP,
				method,
				path,
				comment,
			)

			if statusCode != http.StatusNotFound {
				elapsed := latency.Seconds() * 1000.0
				ResponseCounter.WithLabelValues(method, path, instanceId).Inc()
				ErrorCounter.WithLabelValues(strconv.FormatInt(int64(statusCode), 10),
					fmt.Sprintf("%s-%s", path, method), instanceId).Inc()
				ResponseLatency.WithLabelValues(method, path, instanceId).Observe(elapsed)
			}
		}
	}
}

func GetMetrics(c *gin.Context) {
	handler := promhttp.Handler()
	handler.ServeHTTP(c.Writer, c.Request)
}
