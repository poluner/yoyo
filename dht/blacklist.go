package dht

import (
	"sync"
	"time"
)

// blockedItem represents a blocked node.
type blockedItem struct {
	ip         string
	port       int
	createTime time.Time
}

// blackList manages the blocked nodes including which sends bad information
// and can't ping out.
type blackList struct {
	*sync.Mutex
	list         map[string]blockedItem
	maxSize      int
	expiredAfter time.Duration
}

// newBlackList returns a blackList pointer.
func newBlackList(size int) *blackList {
	return &blackList{
		Mutex:        &sync.Mutex{},
		list:         make(map[string]blockedItem),
		maxSize:      size,
		expiredAfter: time.Hour * 1,
	}
}

// genKey returns a key. If port is less than 0, the key wil be ip. Ohterwise
// it will be `ip:port` format.
func (bl *blackList) genKey(ip string, port int) string {
	key := ip
	if port >= 0 {
		key = genAddress(ip, port)
	}
	return key
}

// insert adds a blocked item to the blacklist.
func (bl *blackList) insert(ip string, port int) {
	bl.Lock()
	defer bl.Unlock()

	if len(bl.list) >= bl.maxSize {
		return
	}

	bl.list[bl.genKey(ip, port)] = blockedItem{
		ip:         ip,
		port:       port,
		createTime: time.Now(),
	}
}

// delete removes blocked item form the blackList.
func (bl *blackList) delete(ip string, port int) {
	bl.Lock()
	defer bl.Unlock()

	delete(bl.list, bl.genKey(ip, port))
}

// validate checks whether ip-port pair is in the block nodes list.
func (bl *blackList) in(ip string, port int) bool {
	bl.Lock()
	defer bl.Unlock()

	if _, ok := bl.list[ip]; ok {
		return true
	}

	key := bl.genKey(ip, port)
	v, ok := bl.list[key]
	if ok {
		if time.Now().Sub(v.createTime) < bl.expiredAfter {
			return true
		}
		delete(bl.list, bl.genKey(ip, port))
	}
	return false
}

// clear cleans the expired items every 10 minutes.
func (bl *blackList) clear() {
	for range time.Tick(time.Minute * 10) {
		keys := make([]string, 0, 100)
		bl.Lock()

		for key, val := range bl.list {
			if time.Now().Sub(val.createTime) > bl.expiredAfter {
				keys = append(keys, key)
			}
		}

		for _, key := range keys {
			delete(bl.list, key)
		}
		bl.Unlock()
	}
}
