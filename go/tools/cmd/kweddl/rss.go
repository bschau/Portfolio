package main

import (
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"

	"github.com/ungerik/go-rss"
)

// RssGetMelodies - get any unseen melodies from KWED rss feed
func RssGetMelodies(baseDomain string) []KwedMelody {
	feedXML, err := rss.Read("http://"+baseDomain+"/rss.xml", false)
	if err != nil {
		log.Fatal("Failed to retrieve rss from " + baseDomain + " err: " + err.Error())
	}

	channel, err := rss.Regular(feedXML)
	if err != nil {
		log.Fatal("Failed to regular from " + baseDomain + " err: " + err.Error())
	}

	var melodies []KwedMelody
	for _, item := range channel.Item {
		pos := strings.LastIndex(item.Link, "/")
		if pos == -1 {
			fmt.Fprintln(os.Stderr, "Index malformed:", item.Link)
			continue
		}

		pos++
		songID, err := strconv.Atoi(item.Link[pos:])
		if err != nil {
			fmt.Fprintln(os.Stderr, "Cannot convert item to songID:", item.Link[pos:])
			continue
		}

		if CounterSeen(songID) {
			fmt.Println("Index already seen:", songID)
			continue
		}

		melody := new(KwedMelody)
		melody.SongID = songID
		melody.Link = item.Link
		melody.Title = item.Title

		melodies = append(melodies, *melody)
	}

	return melodies
}
