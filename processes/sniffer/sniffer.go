package main

import (
	"fmt"
	"github.com/LiuRoy/yoyo/dht"
)

func main() {
	config := dht.NewCrawlConfig()
	config.OnAnnouncePeer = func(infoHash, ip string, port int) {
		fmt.Printf("%s\t%s\t%d\n", infoHash, ip, port)
	}
	d := dht.New(config)

	d.Run()
}
