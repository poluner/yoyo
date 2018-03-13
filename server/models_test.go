package server

import (
	"fmt"
	"testing"
)

func TestEsSuggest(t *testing.T) {
	result, err := EsSuggest("fosage", 2)
	if err != nil {
		t.Fail()
	}

	fmt.Println(result)
}

func TestEsSearch(t *testing.T) {
	total, result, err := EsSearch("batman", 0, 1)
	if err != nil {
		t.Fail()
	}

	fmt.Println(total)
	fmt.Printf("%+v\n", result)
}

func TestEsUpdateHot(t *testing.T) {
	err := EsUpdateHot("5d869073b0c6eaaabbdb07e35abbbb35c1ed6ed5")
	if err != nil {
		t.Fail()
	}
}

func TestEsUpdateMetaData(t *testing.T) {
	meta := updatePost{
		Hot: 110,
	}
	err := EsUpdateMetaData("111111", &meta)
	if err != nil {
		t.Fail()
	}
}
