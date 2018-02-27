package server

import (
	"fmt"
	log "github.com/alecthomas/log4go"
	"github.com/gin-gonic/gin"
	"github.com/prometheus/client_golang/prometheus"
	"github.com/prometheus/client_golang/prometheus/promhttp"
	"net/http"
	"strconv"
	"time"
)

var (
	historyBuckets = [...]float64{
		10., 20., 30., 50., 80., 100., 200., 300., 500., 1000., 2000., 3000.}

	ResponseCounter = prometheus.NewCounterVec(prometheus.CounterOpts{
		Name: fmt.Sprintf("%s_requests_total", ProjectName),
		Help: "Total request counts"}, []string{"method", "endpoint"})
	ErrorCounter = prometheus.NewCounterVec(prometheus.CounterOpts{
		Name: fmt.Sprintf("%s_error_total", ProjectName),
		Help: "Total Error counts"}, []string{"method", "endpoint"})
	ResponseLatency = prometheus.NewHistogramVec(prometheus.HistogramOpts{
		Name:    fmt.Sprintf("%s_response_latency_millisecond", ProjectName),
		Help:    "Response latency (millisecond)",
		Buckets: historyBuckets[:]}, []string{"method", "endpoint"})
)

func init() {
	prometheus.MustRegister(ResponseCounter)
	prometheus.MustRegister(ErrorCounter)
	prometheus.MustRegister(ResponseLatency)
}

func Metrics(notLogged ...string) gin.HandlerFunc {
	var skip map[string]struct{}

	if length := len(notLogged); length > 0 {
		skip = make(map[string]struct{}, length)

		for _, path := range notLogged {
			skip[path] = struct{}{}
		}
	}

	return func(c *gin.Context) {
		// Start timer
		start := time.Now()
		path := c.Request.URL.Path

		// Process request
		c.Next()

		// Log only when path is not being skipped
		if _, ok := skip[path]; !ok {
			// Stop timer
			end := time.Now()
			latency := end.Sub(start)

			clientIP := c.ClientIP()
			method := c.Request.Method
			statusCode := c.Writer.Status()
			comment := c.Errors.ByType(gin.ErrorTypePrivate).String()

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
				ResponseCounter.WithLabelValues(method, path).Inc()
				ErrorCounter.WithLabelValues(strconv.FormatInt(int64(statusCode), 10),
					fmt.Sprintf("%s-%s", path, method)).Inc()
				ResponseLatency.WithLabelValues(method, path).Observe(elapsed)
			}
		}
	}
}

func GetMetrics(c *gin.Context) {
	handler := promhttp.Handler()
	handler.ServeHTTP(c.Writer, c.Request)
}
