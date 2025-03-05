package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"github.com/gorilla/feeds"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"time"
)

// LichessOpponent is an opponent
type LichessOpponent struct {
	Username string
}

// LichessGame is a single game
type LichessGame struct {
	FullID   string
	Opponent LichessOpponent
	IsMyTurn bool
}

// LichessResponse is the overall response from the Lichess API
type LichessResponse struct {
	NowPlaying []LichessGame
}

func lichess() string {
	token := getToken()
	games := getGames(token)

	feed := &feeds.Feed {
		Title:          "Lichess",
		Link:           &feeds.Link { Href: "https://lichess.org" },
		Description:    "Is it my turn?",
		Created:        time.Now(),
	}

	feed.Items = []*feeds.Item{}
	recordGames(feed, games)

	r := &feeds.Rss { feed }
	r.RssFeed().Ttl = 480
	rss, err := feeds.ToXML(r)

	if err != nil {
		log.Fatal(err)
	}

	return rss
}

func getToken() string {
	home := os.Getenv("HOME")
	file, _ := os.Open(home + "/.lichesst")
	defer file.Close()

	reader := bufio.NewReader(file)
	result, _ := reader.ReadString('\n')
	return result[:len(result) - 1]
}

func getGames(token string) []LichessGame {
	bearer := "Bearer " + token
	req, err := http.NewRequest("GET", "https://lichess.org/api/account/playing", nil)
	if err != nil {
		log.Fatal("Http Request - new: ", err)
	}

	req.Header.Add("Authorization", bearer)
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		log.Fatal("Http Request - read: ", err)
	}

	body, _ := ioutil.ReadAll(resp.Body)

	var response LichessResponse
	err = json.Unmarshal([]byte(body), &response)
	if err != nil {
		log.Fatal("JSON Decode: ", err)
	}

	return response.NowPlaying
}

func needToSendEmail(games []LichessGame) bool {
	if games == nil || len(games) == 0 {
		return false
	}

	waitingOnMe := 0
	for _, game := range games {
		if game.IsMyTurn {
			waitingOnMe++
		}
	}

	return waitingOnMe > 0
}

func recordGames(feed *feeds.Feed, games []LichessGame) {
	if games == nil || len(games) == 0 {
		item := &feeds.Item {
			Title:          "No games",
			Link:           &feeds.Link { Href: "https://lichess.org" },
			Description:    "No ongoing game, please challenge someone.",
			Created:        time.Now(),
		}

		feed.Add(item)
		return
	}

	var myTurn string
	var link string

	for _, game := range games {
		if game.IsMyTurn {
			myTurn = "<b>YES</b>"
			link = fmt.Sprintf(" (<a href=\"https://lichess.org/" + game.FullID + "\">Link</a>)")
		} else {
			myTurn = "no"
			link = ""
		}

		output := fmt.Sprintf("<li>My turn in game vs. <i>" + game.Opponent.Username + "</i>: " + myTurn + link + "</li>")
		item := &feeds.Item {
			Title:          "Game vs. " + game.Opponent.Username,
			Link:           &feeds.Link { Href: "https://lichess.org/" + game.FullID },
			Description:    output,
			Created:        time.Now(),
		}

		feed.Add(item)
	}
}
