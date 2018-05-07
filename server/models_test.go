package server

import (
	"context"
	"fmt"
	"testing"
)

func TestEsSuggest(t *testing.T) {
	result, err := EsSuggest(context.Background(), "fosage", 2)
	if err != nil {
		t.Fail()
	}

	fmt.Println(result)
}

func TestEsSearch(t *testing.T) {
	total, result, err := EsSearch(context.Background(), "batman", 0, 1)
	if err != nil {
		t.Fail()
	}

	fmt.Println(total)
	fmt.Printf("%+v\n", result)
}

func TestEsUpdateMetaData(t *testing.T) {
	meta := updatePost{
		Hot: 110,
	}
	err := EsUpdateMetaData(context.Background(), "111111", &meta)
	if err != nil {
		t.Fail()
	}
}
