package newsfeed

import (
	"github.com/ungerik/go-rss"
)

// RssGetRegular - get content of regular feed
func RssGetRegular(url string) (string, []NewsfeedItem) {
	var items []NewsfeedItem
	feedXML, err := rss.Read(url, false)
	if err != nil {
		msg := "Failed to retrieve rss from " + url + " err: " + err.Error()
		return msg, items
	}

	channel, err := rss.Regular(feedXML)
	if err != nil {
		msg := "Failed to regular from " + url + " err: " + err.Error()
		return msg, items
	}

	for _, item := range channel.Item {
		newsfeedItem := new(NewsfeedItem)
		newsfeedItem.Title = item.Title
		newsfeedItem.Link = item.Link

		items = append(items, *newsfeedItem)
	}

	return "", items
}
