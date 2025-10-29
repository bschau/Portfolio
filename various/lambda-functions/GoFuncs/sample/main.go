package main

import (
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
		runGoSample()
	} else {
		lambda.Start(handleRequest)
	}
}

func runGoSample() {
	now := time.Now().Local()
	nowStr := fmt.Sprintf("%02d/%02d/%02d %02d:%02d:%02d",
		now.Year(), now.Month(), now.Day(),
		now.Hour(), now.Minute(), now.Second())
	title := "GoSample " + nowStr
	sendMail(getSendgrid(), title, title)
}

func handleRequest() (string, error) {
	runGoSample()
        return "OK", nil
}

func guessEnv() {
	if len(os.Args) > 1 && os.Args[1] == "cli" {
		cliEnv = true
		return
	}

	cliEnv = false
}

func getSendgrid() SendgridRc {
	filename := getSendgridRcFilename()
	raw, err := ioutil.ReadFile(filename)
	if err != nil {
		log.Fatal(err)
	}

	result := SendgridRc{}
	json.Unmarshal(raw, &result)
	return result
}

func getSendgridRcFilename() string {
	if !cliEnv {
		return "sendgridrc"
	}

	home, err := os.UserHomeDir()
	if err != nil {
		log.Fatal(err)
	}
	return home + "/.sendgridrc"
}

func sendMail(sendgridRc SendgridRc, title string, body string) {
	from := mail.NewEmail(sendgridRc.FromName, sendgridRc.From)
	to := mail.NewEmail(sendgridRc.ToName, sendgridRc.To)
	plainTextContent := "View this in an HTML capable email-client"
	message := mail.NewSingleEmail(from, title, to, plainTextContent, body)
	client := sendgrid.NewSendClient(sendgridRc.APIKey)
	_, err := client.Send(message)
	if err != nil {
		log.Fatal(err)
	}
}
