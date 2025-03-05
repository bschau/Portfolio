package main

import (
	"github.com/gorilla/feeds"
	"log"
	"strings"
	"strconv"
	"time"
)

func comics(host string) string {
	feed := &feeds.Feed {
		Title:		"Comics",
		Link:		&feeds.Link { Href: "http://" + host },
		Description:	"A collection of favorite comics",
		Created:	time.Now(),
	}

	feed.Items = []*feeds.Item{}

	creators := "class=\"fancybox\""
	creatorsAndThen := "img src"
	fetchGeneric(feed, "Andy Capp", "https://www.creators.com/read/andy-capp", creators, creatorsAndThen, 0)
	fetchGeneric(feed, "B.C.", "https://www.creators.com/read/bc", creators, creatorsAndThen, 0)
	fetchComicsKingdom(feed, "Hagar the Horrible", "https://www.comicskingdom.com/hagar-the-horrible")
	fetchComicsKingdom(feed, "Zits", "https://www.comicskingdom.com/zits")

	r := &feeds.Rss { feed }
	r.RssFeed().Ttl = 1440
	rss, err := feeds.ToXML(r)

	if err != nil {
		log.Fatal(err)
	}

	return rss
}

func fetchGeneric(feed *feeds.Feed, title string, url string, origin string, andThen string, width int) {
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

	data :=
		"<h1><a href=\"" + url + "\">" + title + "</a></h1>" +
		"<p><img src=\"" + html + "\" alt=\"" + title + "\""

	if width > 0 {
		data += " width=\"" + strconv.Itoa(width) + "\""
	}
	data += " /></p><p><br /></p>"

	item := &feeds.Item {
		Title:		title,
		Link:		&feeds.Link { Href: url },
		Description:	data,
		Created:	time.Now(),
	}

	feed.Add(item)
}

func fetchComicsKingdom(feed *feeds.Feed, title string, url string) {
	html := GetURL(url)
	if len(html) < 1 {
		return
	}
	pos := strings.Index(html, "data-comic-image-url")
	html = html[pos+22:]
	pos = strings.Index(html, "\"")
	html = html[:pos]

	data :=
		"<h1><a href=\"" + url + "\">" + title + "</a></h1>" +
		"<p><img src=\"" + html + "\" alt=\"" + title + "\"" +
		" width=\"900\" /></p><p><br /></p>"

	item := &feeds.Item {
		Title:		title,
		Link:		&feeds.Link { Href: url },
		Description:	data,
		Created:	time.Now(),
	}

	feed.Add(item)
}
