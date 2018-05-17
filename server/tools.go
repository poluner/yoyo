package server

import (
	"fmt"
	"net"
	"net/http"
	"strconv"
	"strings"
	"time"
)

type JsonTime struct {
	time.Time
}

func (t *JsonTime) UnmarshalJSON(b []byte) (err error) {
	s := string(b)
	if s == "null" {
		t.Time = time.Time{}
		return
	}

	i, err := strconv.ParseInt(s, 10, 64)
	if err == nil {
		t.Time = time.Unix(i, 0)
		return
	}

	err = nil
	st, err := time.Parse("2006-01-02", strings.Trim(string(b), "\""))
	if err == nil {
		t.Time = st
		return
	}

	return
}

func (t JsonTime) MarshalJSON() ([]byte, error) {
	return []byte(fmt.Sprintf("%s", strconv.FormatInt(t.Time.Unix(), 10))), nil
}

func clientIP(r *http.Request) (string, bool) {
	forwardedFor := r.Header.Get("X-Forwarded-For")
	if forwardedFor != "" {
		return strings.TrimSpace(strings.Split(forwardedFor, ",")[0]), true
	}
	ip, _, err := net.SplitHostPort(r.RemoteAddr)
	if err != nil {
		return r.RemoteAddr, false
	}
	return ip, false
}

func btoi(b bool) int {
	if b {
		return 1
	}
	return 0
}
