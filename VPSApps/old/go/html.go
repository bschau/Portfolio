package main

import (
	"fmt"
	"strings"
	"time"
)

var builder strings.Builder

// Title - the title used in the HTML
var Title string

// Start - start HTML rendering
func Start(tag string) {
	now := time.Now()
	niceTime := fmt.Sprintf("%d-%02d-%02d %02d:%02d:%02d",
		now.Year(), now.Month(), now.Day(),
		now.Hour(), now.Minute(), now.Second())
	Title = "[" + tag + "] " + niceTime

	html :=
		"<!DOCTYPE html>" +
			"<html dir=\"ltr\" lang=\"en\">" +
			"<head>" +
			"  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">" +
			"  <meta name=\"viewport\" content=\"width=device-width\">" +
			"  <title>" + Title + "</title>" +
			"</head>" +
			"<body>"
	builder.WriteString(html)
}

// End - end HTML rendering
func End() {
	builder.WriteString("</body></html>")
}

// ToString - return HTML string
func ToString() string {
	return builder.String()
}
