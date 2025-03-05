package lichess

import (
	"bufio"
	"encoding/json"
	"fmt"
	"internal/common"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"time"
)

// Lichess Run lichess module
func Lichess(home string) {
	token := getToken(home)
	games := getGames(token)
	if !needToSendEmail(games) {
		return
	}

	buildHTML(games)
}

func getToken(home string) string {
	file, _ := os.Open(home + "/.lichesst")
	defer file.Close()

	reader := bufio.NewReader(file)
	result, _ := reader.ReadString('\n')
	return result[:len(result)-1]
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

func buildHTML(games []LichessGame) {
	var myTurn string
	var link string

	common.HTMLAddBlock("<p>My turn in game vs.:</p>")

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
		common.HTMLAddBlock(output)
	}
}
