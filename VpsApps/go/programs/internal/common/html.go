package common

import "strings"

var sb strings.Builder
var lineNo int
var emittable bool

// HTMLToString Return string representation of builder
func HTMLToString() string {
	return sb.String()
}

// HTMLInit Initialize HTML builder
func HTMLInit(title string) {
	emittable = false
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

// HTMLEnd Stop HTML
func HTMLEnd() {
	sb.WriteString("</body></html>")
}

// HTMLError Add error
func HTMLError(error string) {
	sb.WriteString("<p style=\"color: red\">")
	sb.WriteString(error)
	sb.WriteString("</p><p><br /></p>")
	emittable = true
}

// HTMLPre Prepare for items
func HTMLPre(title string, link string) {
	lineNo = 0
	sb.WriteString("<h1><a href=\"")
	sb.WriteString(link)
	sb.WriteString("\">")
	sb.WriteString(title)
	sb.WriteString("</a></h1>")
	sb.WriteString("<table style=\"width: 100%\">")
}

// HTMLPost End
func HTMLPost() {
	sb.WriteString("</table><p><br /></p>")
}

// HTMLAddItem Add item to current builder
func HTMLAddItem(link string, title string) {
	sb.WriteString("<tr><td style=\"background-color: ")
	if lineNo%2 == 1 {
		sb.WriteString("#efe")
	} else {
		sb.WriteString("#fff")
	}

	sb.WriteString("; font-size: 120%;\"><a href=\"")
	sb.WriteString(link)
	sb.WriteString("\">")
	sb.WriteString(title)
	sb.WriteString("</a></td></tr>\r\n")
	lineNo++
	emittable = true
}

// HTMLEmittable Is the HTML worth emitting
func HTMLEmittable() bool {
	return emittable
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
	emittable = true
}

// HTMLLineBreak Add line break to current builder
func HTMLLineBreak(breakText string) {
	if len(breakText) == 0 {
		breakText = "<br />"
	}

	sb.WriteString("<p>")
	sb.WriteString(breakText)
	sb.WriteString("</p>\r\n")
	emittable = true
}

// HTMLAddBlock Add custom (formatted) block
func HTMLAddBlock(block string) {
	sb.WriteString(block)
	sb.WriteString("\r\n")
	emittable = true
}
