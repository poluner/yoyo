package yoyo

import (
	"testing"
	"fmt"
)

func TestNode(t *testing.T) {
	_, err := newNode("aaa", "udp", "127.0.0.1:88")
	if err == nil {
		t.Fail()
	}

	node, err := newNode("aaaaaaaaaaaaaaaaaaaa", "udp", "127.0.0.1:88")
	if err != nil {
		t.Fail()
	}

	if node.id.RawString() != "aaaaaaaaaaaaaaaaaaaa" {
		t.Fail()
	}

	node, err = newNodeFromCompactInfo("aaaa", "udp")
	if err == nil {
		t.Fail()
	}

	node, err = newNodeFromCompactInfo("aaaaaaaaaaaaaaaaaaaaabcdef", "udp")
	if err != nil {
		t.Fail()
	}

	fmt.Printf(node.CompactIPPortInfo())
	//
	//if node.CompactIPPortInfo() != "abcdef" {
	//	t.Fail()
	//}
	//
	//if node.CompactNodeInfo() != "aaaaaaaaaaaaaaaaaaaaabcdef" {
	//	t.Fail()
	//}
}
