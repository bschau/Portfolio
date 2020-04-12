package main

import (
	"log"
	"context"
	"github.com/mailgun/mailgun-go"
	"time"
)

// Mailgun is used to send an email using the Mailgun service
func Mailgun(configuration RCFile, subject string, body string) {
	mg := mailgun.NewMailgun(configuration.MGDomain, configuration.MGAuth)
	m := mg.NewMessage(
		"VPSApps <postmaster@" + configuration.MGDomain + ">",
		subject,
		"",
		configuration.MGTo,
	)

	m.SetHtml(body);

	ctx, cancel := context.WithTimeout(context.Background(), time.Second*30)
	defer cancel()

	_, _, err := mg.Send(ctx, m)
	if err != nil {
		log.Fatal(err)
	}
}
