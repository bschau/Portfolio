package common

import (
	"crypto/tls"
	"encoding/json"
	"io/ioutil"
	"log"

	"github.com/go-gomail/gomail"
	"github.com/sendgrid/sendgrid-go"
	"github.com/sendgrid/sendgrid-go/helpers/mail"
)

// DeliveryRc is the delivery configuration
type DeliveryRc struct {
	Type        string
	Filename    string
	From        string
	FromName    string
	To          string
	ToName      string
	SendGridKey string
	SmtpHost    string
	SmtpPort    int
}

var deliveryRc DeliveryRc = DeliveryRc{}

// DeliveryInit Initialize delivery system
func DeliveryInit(home string) {
	filename := home + "/.deliveryrc"
	raw, err := ioutil.ReadFile(filename)
	if err != nil {
		log.Fatal(err)
	}

	json.Unmarshal(raw, &deliveryRc)
}

// Deliver Deliver file to desktop or by mail
func Deliver(subject string, body string) {
	if deliveryRc.Type == "file" {
		sendToDesktop(body)
		return
	} else if deliveryRc.Type == "sendgrid" {
		sendMailBySendGrid(subject, body)
		return
	} else if deliveryRc.Type == "smtp" {
		sendMailBySmtp(subject, body)
		return
	}

	panic("Unknown delivery type: " + deliveryRc.Type)
}

func sendToDesktop(body string) {
	ioutil.WriteFile(deliveryRc.Filename, []byte(body), 0644)
}

func sendMailBySendGrid(title string, html string) {
	from := mail.NewEmail(deliveryRc.FromName, deliveryRc.From)
	to := mail.NewEmail(deliveryRc.ToName, deliveryRc.To)
	plainTextContent := "View this in an HTML capable email-client"
	message := mail.NewSingleEmail(from, title, to, plainTextContent, html)
	client := sendgrid.NewSendClient(deliveryRc.SendGridKey)
	_, err := client.Send(message)
	if err != nil {
		log.Fatal(err)
	}
}

func sendMailBySmtp(title string, html string) {
	dialer := gomail.Dialer{Host: deliveryRc.SmtpHost, Port: deliveryRc.SmtpPort}
	dialer.TLSConfig = &tls.Config{InsecureSkipVerify: true}
	message := gomail.NewMessage()
	from := "\"" + deliveryRc.FromName + "\" <" + deliveryRc.From + ">"
	to := "\"" + deliveryRc.ToName + "\" <" + deliveryRc.To + ">"
	message.SetHeader("From", from)
	message.SetHeader("To", to)
	message.SetHeader("Subject", title)
	message.SetBody("text/html", html)
	if err := dialer.DialAndSend(message); err != nil {
		log.Fatal(err)
	}
}
