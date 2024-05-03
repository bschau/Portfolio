package main

import (
	"fmt"
	"log"
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
		fmt.Println("Fetching:", url)
		mp3 := FetchURL(url)
		if mp3 == nil {
			fmt.Fprintln(os.Stderr, "Failed to fetch", url)
			continue
		}

		filename := getTitle(melody) + ".mp3"
		os.WriteFile(filename, mp3, 0644)
		CounterMark(melody.SongID)
	}

	CounterSave()
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
