package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"time"

	"github.com/aws/aws-lambda-go/lambda"
	"github.com/sendgrid/sendgrid-go"
	"github.com/sendgrid/sendgrid-go/helpers/mail"
)

var cliEnv bool

func main() {
	guessEnv()

	if cliEnv {
		runLichessMoves()
	} else {
		lambda.Start(handleRequest)
	}
}

func guessEnv() {
	if len(os.Args) > 1 && os.Args[1] == "cli" {
		cliEnv = true
		return
	}

	cliEnv = false
}

func handleRequest() (string, error) {
	runLichessMoves()
	return "OK", nil
}

func getFullPath(filename string) string {
	if !cliEnv {
		return filename
	}

	home, err := os.UserHomeDir()
	if err != nil {
		log.Fatal(err)
	}

	return home + "/." + filename
}

func runLichessMoves() {
	now := time.Now().Local()
	nowStr := fmt.Sprintf("%02d/%02d/%02d %02d:%02d:%02d",
		now.Year(), now.Month(), now.Day(),
		now.Hour(), now.Minute(), now.Second())
	title := "Lichess " + nowStr
	gamesHTML := lichess(getToken(), title)
	if len(gamesHTML) > 0 {
		sendMail(getSendgrid(), title, gamesHTML)
	}
}

func getToken() string {
	file, _ := os.Open(getFullPath("lichesst"))
	defer file.Close()

	reader := bufio.NewReader(file)
	result, _ := reader.ReadString('\n')
	return result[:len(result)-1]
}

func getSendgrid() SendgridRc {
	raw, _ := ioutil.ReadFile(getFullPath("sendgridrc"))
	result := SendgridRc{}
	json.Unmarshal(raw, &result)
	return result
}

func sendMail(sendgridRc SendgridRc, title string, gamesHTML string) {
	from := mail.NewEmail(sendgridRc.FromName, sendgridRc.From)
	to := mail.NewEmail(sendgridRc.ToName, sendgridRc.To)
	plainTextContent := "View this in an HTML capable email-client"
	message := mail.NewSingleEmail(from, title, to, plainTextContent, gamesHTML)
	client := sendgrid.NewSendClient(sendgridRc.APIKey)
	_, err := client.Send(message)
	if err != nil {
		log.Fatal(err)
	}
}
