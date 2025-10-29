package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"strings"
	"time"
)

func lichess(token string, title string) string {
	games := getGames(token)
	if !needToSendEmail(games) {
		return ""
	}

	return buildHTML(games, title)
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

func buildHTML(games []LichessGame, title string) string {
	var sb strings.Builder
	var myTurn string
	var link string

	sb.WriteString(`<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
	<title>`)
	sb.WriteString(title)
	sb.WriteString(`</title>
</head>
<body>
<p>My turn in game vs.:</p>
<ul>`)

	for _, game := range games {
		url := fmt.Sprintf("https://lichess.org/%s?bsts=%d", game.FullID, time.Now().Unix())
		if game.IsMyTurn {
			myTurn = "<b>YES</b>"
			link = fmt.Sprintf(" (<a href=\"%s\">Link</a>)", url)
		} else {
			myTurn = "no"
			link = ""
		}

		output := fmt.Sprintf("<li>My turn in game vs. <i>" + game.Opponent.Username + "</i>: " + myTurn + link + "</li>")
		sb.WriteString(output)
	}

	sb.WriteString("</ul></body></html>")
	return sb.String()
}
