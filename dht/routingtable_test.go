package dht

import (
	"strings"
	"testing"
	"time"
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

	if node.CompactNodeInfo() != "aaaaaaaaaaaaaaaaaaaaabcdef" {
		t.Fail()
	}
}

func TestBucket(t *testing.T) {
	prefix := newBitmapFromString("aaaaa")
	bucket := newKBucket(prefix)

	childrenId := bucket.RandomChildID()
	if !strings.HasPrefix(childrenId, "aaaaa") {
		t.Fail()
	}

	node1, _ := newNode("aaaaaaaaaaaaaaaaaaaa", "udp", "127.0.0.1:88")
	node2, _ := newNode("bbbbbbbbbbbbbbbbbbbb", "udp", "127.0.0.1:88")
	node3, _ := newNode("cccccccccccccccccccc", "udp", "127.0.0.1:88")
	if !bucket.Insert(node1) {
		t.Fail()
	}
	if !bucket.Insert(node2) {
		t.Fail()
	}
	if bucket.Insert(node1) {
		t.Fail()
	}

	node3.lastActiveTime = time.Now().Add(-1 * time.Hour)
	bucket.candidates.Push(node3.id.RawString(), node3)
	bucket.Replace(node1)
	if bucket.nodes.Front().Value.(*node).id.RawString() != node3.id.RawString() {
		t.Fail()
	}
}
