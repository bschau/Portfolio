package main

import (
	"fmt"
	"internal/comics"
	"internal/common"
	"internal/lichess"
	"internal/newsfeed"
	"log"
	"os"
	"time"
)

func main() {
	if len(os.Args) != 2 {
		panic("You must specify module to run: comics | lichess | newsfeed")
	}

	now := time.Now().Local()
	nowStr := fmt.Sprintf("%02d/%02d/%02d %02d:%02d:%02d",
		now.Year(), now.Month(), now.Day(),
		now.Hour(), now.Minute(), now.Second())

	home, err := os.UserHomeDir()
	if err != nil {
		log.Fatal(err)
	}

	var title string
	if os.Args[1] == "comics" {
		title = "Comics " + nowStr
		common.HTMLInit(title)
		comics.Comics()
	} else if os.Args[1] == "lichess" {
		title = "Lichess " + nowStr
		common.HTMLInit(title)
		lichess.Lichess(home)
	} else if os.Args[1] == "newsfeed" {
		title = "Newsfeed " + nowStr
		common.HTMLInit(title)
		newsfeed.Newsfeed(home)
	} else {
		panic("Needs one of comics, lichess or newsfeed")
	}

	common.HTMLEnd()
	if common.HTMLEmittable() {
		common.DeliveryInit(home)
		common.Deliver(title, common.HTMLToString())
	}
}
