package dht

import (
	"net"
	"testing"
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

func TestPeersManager(t *testing.T) {
	dht := New(nil)
	dht.K = 2
	pm := newPeersManager(dht)

	p1 := newPeer(net.IP{91, 98, 99, 100}, 100, "aa")
	p2 := newPeer(net.IP{91, 98, 99, 100}, 101, "bb")
	p3 := newPeer(net.IP{91, 98, 99, 100}, 102, "cc")

	pm.Insert("a", p1)
	pm.Insert("a", p2)
	pm.Insert("a", p3)

	ps := pm.GetPeers("a", 5)
	if len(ps) != 2 {
		t.Fail()
	}

	if ps[0].Port != 101 || ps[0].token != "bb" {
		t.Fail()
	}
}
