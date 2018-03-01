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
