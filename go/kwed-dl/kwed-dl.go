package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"regexp"
	"strconv"
	"strings"
)

var home string

func main() {
	EnvironmentInit()
	CounterLoad()

	const baseDomain = "remix.kwed.org"
	melodies := RssGetMelodies(baseDomain)

	if len(melodies) == 0 {
		os.Exit(0)
	}

	err := os.Chdir(Desktop)
	if err != nil {
		log.Fatal(err)
	}

	for _, melody := range melodies {
		url := "http://" + baseDomain + "/download.php/" + strconv.Itoa(melody.SongID)
		mp3 := fetch(url)
		if mp3 == nil {
			fmt.Fprintln(os.Stderr, "Failed to fetch", url)
			continue
		}

		filename := getTitle(melody) + ".mp3"
		ioutil.WriteFile(filename, mp3, 0644)
		CounterRecord(melody.SongID)
	}
}

func fetch(url string) []byte {
	resp, err := http.Get(url)
	if err != nil {
		return nil
	}
	defer resp.Body.Close()
	data, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return nil
	}
	return data
}

func getTitle(melody KwedMelody) string {
	const prefix = "new c64 remix released: "
	prefixLen := len(prefix)

	regex := regexp.MustCompile("[^a-zA-Z0-9_()+-]")

	title := strings.ToLower(melody.Title)
	if title[:prefixLen] == prefix {
		return regex.ReplaceAllString(melody.Title[prefixLen:], "_")
	}

	return regex.ReplaceAllString(melody.Title, "_")
}
