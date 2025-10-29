package main

import "strings"

var sb strings.Builder

// HTMLToString Return string representation of builder
func HTMLToString() string {
	return sb.String()
}

// HTMLInit Initialize HTML builder
func HTMLInit(title string) {
	sb.WriteString(`<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
	<title>`)
	sb.WriteString(title)
	sb.WriteString(`</title>
</head>
<body>`)
}

// HTMLHeader Add header to current builder
func HTMLHeader(title string, link string) {
	sb.WriteString("<h1><a href=\"")
	sb.WriteString(link)
	sb.WriteString("\">")
	sb.WriteString(title)
	sb.WriteString("</a></h1>")
}

// HTMLAddComic Add comic to current builder
func HTMLAddComic(title string, link string, width string) {
	sb.WriteString("<p><img src=\"")
	sb.WriteString(link)
	sb.WriteString("\" alt=\"")
	sb.WriteString(title)
	sb.WriteString("\"")
	if len(width) > 0 {
		sb.WriteString(" width=\"")
		sb.WriteString(width)
		sb.WriteString("\"")
	}

	sb.WriteString(" /></p>")
}

// HTMLLineBreak Add line break to current builder
func HTMLLineBreak(breakText string) {
	if len(breakText) == 0 {
		breakText = "<br />"
	}

	sb.WriteString("<p>")
	sb.WriteString(breakText)
	sb.WriteString("</p>\r\n")
}
