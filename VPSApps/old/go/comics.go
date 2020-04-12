package main

import (
	"strconv"
	"strings"
)

func comics(configuration RCFile) {
	Start("Comics")
	creators := "class=\"fancybox\""
	creatorsAndThen := "img src"
	fetchGeneric("Andy Capp", "https://www.creators.com/read/andy-capp", creators, creatorsAndThen, 0)
	fetchGeneric("B.C.", "https://www.creators.com/read/bc", creators, creatorsAndThen, 0)
	fetchComicsKingdom("Hagar the Horrible", "https://www.comicskingdom.com/hagar-the-horrible")
	fetchComicsKingdom("Zits", "https://www.comicskingdom.com/zits")
	End()
	Deliver(configuration, Title, ToString())
}

func fetchGeneric(title string, url string, origin string, andThen string, width int) {
	html := GetURL(url)
	if len(html) < 1 {
		return
	}
	pos := strings.Index(html, origin)
	html = html[pos:]
	pos = strings.Index(html, andThen)
	pos += len(andThen) + 2
	html = html[pos:]
	pos = strings.Index(html, "\"")
	html = html[:pos]
	if len(html) < 2 {
		return
	}
	if html[:2] == "//" {
		html = "http:" + html
	}
	builder.WriteString("<h1><a href=\"" + url + "\">" + title + "</a></h1>")
	builder.WriteString("<p><img src=\"" + html + "\" alt=\"" + title + "\"")
	if width > 0 {
		builder.WriteString(" width=\"" + strconv.Itoa(width) + "\"")
	}
	builder.WriteString(" /></p><p><br /></p>")
}

func fetchComicsKingdom(title string, url string) {
	html := GetURL(url)
	if len(html) < 1 {
		return
	}
	pos := strings.Index(html, "data-comic-image-url")
	html = html[pos+22:]
	pos = strings.Index(html, "\"")
	html = html[:pos]
	builder.WriteString("<h1><a href=\"" + url + "\">" + title + "</a></h1>")
	builder.WriteString("<p><img src=\"" + html + "\" alt=\"" + title + "\" width=\"900\" /></p><p><br /></p>")
}
