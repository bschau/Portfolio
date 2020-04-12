package main

import (
	"crypto/tls"
	"encoding/base64"
	"log"
	"net/smtp"
	"strconv"
	"strings"
)

// SMTP is used to send an email to a recipient
func SMTP(configuration RCFile, subject string, body string) {
	tlsConfig := &tls.Config{
		InsecureSkipVerify: true,
		ServerName:         configuration.MX,
	}

	c, err := smtp.Dial(configuration.MX + ":" + strconv.Itoa(configuration.MXPort))
	if err != nil {
		log.Fatal(err)
	}

	defer c.Close()

	if configuration.MXPort == 587 {
		c.StartTLS(tlsConfig)
	}

	from := getMailbox(configuration.From)
	if err = c.Mail(from); err != nil {
		log.Fatal(err)
	}

	to := getMailbox(configuration.To)
	if err = c.Rcpt(to); err != nil {
		log.Fatal(err)
	}

	w, err := c.Data()
	if err != nil {
		log.Fatal(err)
	}
	defer w.Close()

	var chunker strings.Builder
	msg := "From: " + configuration.From + "\r\n" +
		"To: " + configuration.To + "\r\n" +
		"Subject: " + subject + "\r\n" +
		"Content-Type: text/html; charset=utf-8\r\n" +
		"Content-Transfer-Encoding: base64\r\n" +
		"\r\n"
	chunker.WriteString(msg)

	payload := base64.StdEncoding.EncodeToString([]byte(body))
	length := 76
	for len(payload) > 0 {
		if len(payload) < 76 {
			length = len(payload)
		} else {
			length = 76
		}

		chunk := payload[:length]
		chunker.WriteString(chunk)
		chunker.WriteString("\r\n")
		payload = payload[length:]
	}

	msg = chunker.String()

	_, err = w.Write([]byte(msg))
	if err != nil {
		log.Fatal(err)
	}

	err = w.Close()
	if err != nil {
		log.Fatal(err)
	}

	if err = c.Quit(); err != nil {
		log.Fatal(err)
	}
}

func getMailbox(src string) string {
	pos := strings.Index(src, "<")
	if pos < 0 {
		return src
	}

	src = src[pos+1:]
	pos = strings.Index(src, ">")
	return src[:pos]
}
