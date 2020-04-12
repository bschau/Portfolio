package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
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

func lichess(configuration RCFile) {
	games := getGames(configuration.LichessToken)
	if !needToSendEmail(games) {
		return
	}

	Start("Lichess")
	recordGames(games)
	End()
	Deliver(configuration, Title, ToString())
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

func recordGames(games []LichessGame) {
	var myTurn string
	var link string

	builder.WriteString("<ul>")
	for _, game := range games {
		if game.IsMyTurn {
			myTurn = "<b>YES</b>"
			link = fmt.Sprintf(" (<a href=\"https://lichess.org/" + game.FullID + "\">Link</a>)")
		} else {
			myTurn = "no"
			link = ""
		}

		output := fmt.Sprintf("<li>My turn in game vs. <i>" + game.Opponent.Username + "</i>: " + myTurn + link + "</li>")
		builder.WriteString(output)
	}
	builder.WriteString("</ul>")
}
