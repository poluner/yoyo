package server

import (
	"fmt"
	"strconv"
	"strings"
	"time"
)

type JsonTime struct {
	time.Time
}

func (t *JsonTime) UnmarshalJSON(b []byte) error {
	s := strings.Trim(string(b), "\"")
	if s == "null" {
		t.Time = time.Time{}
		return nil
	}

	tm, err := time.Parse("2006-01-02T15:04:05.999999", s)
	if err != nil {
		return err
	}

	t.Time = tm
	return nil
}

func (t JsonTime) MarshalJSON() ([]byte, error) {
	return []byte(fmt.Sprintf("%s", strconv.FormatInt(t.Time.Unix(), 10))), nil
}
