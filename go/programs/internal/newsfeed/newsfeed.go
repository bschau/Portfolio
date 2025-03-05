package newsfeed

import (
	"internal/common"
	"time"
)

var home string
var entries int

// Newsfeed Run newsfeed module
func Newsfeed(home string) {
	now := time.Now().Local()
	CacheInit(home+"/.newsfeed.gob", now)
	CacheLoad()

	handleFeed("The Daily WTF", "http://syndication.thedailywtf.com/TheDailyWtf", " http://thedailywtf.com/")
	handleFeed("DR Nyheder", "http://www.dr.dk/nyheder/service/feeds/allenyheder", "http://www.dr.dk/nyheder/?rss=true")
	handleFeed("DR Sport", "http://www.dr.dk/nyheder/service/feeds/sporten", "http://www.dr.dk/sporten/")
	handleFeed("TV 2 Sport", "http://feeds.tv2.dk/sporten_seneste/rss", "http://feeds.tv2.dk/sporten_seneste/atom")
	handleFeed("Android Authority", "https://www.androidauthority.com/feed/", "https://www.androidauthority.com/")
	handleFeed("Computerworld", "https://www.computerworld.com/index.rss", "https://www.computerworld.com/")
	handleFeed("GSM Arena", "http://www.gsmarena.com/rss-news-reviews.php3", "http://www.gsmarena.com/")
	handleFeed("Linux Today", "https://feeds.feedburner.com/linuxtoday/linux", "http://www.linuxtoday.com/")
	handleFeed("Mobil.nu", "https://mobil.nu/feed", "https://mobil.nu/")
	handleFeed("OMG! Ubuntu!", "http://feeds.feedburner.com/d0od", "http://www.omgubuntu.co.uk/")
	handleFeed("PCWorld", "https://www.pcworld.com/index.rss", "http://www.pcworld.com/")

	CachePrune()
	CacheSave()
}

func handleFeed(title string, rssURL string, homeURL string) {
	err, feeds := RssGetRegular(rssURL)
	if len(err) > 0 {
		common.HTMLError(err)
		return
	}

	if len(feeds) == 0 {
		return
	}

	var items []NewsfeedItem
	for _, item := range feeds {
		if CacheHas(item.Link) {
			continue
		}

		items = append(items, item)
	}

	if len(items) == 0 {
		return
	}

	common.HTMLPre(title, homeURL)
	for _, item := range items {
		common.HTMLAddItem(item.Link, item.Title)
		CacheAdd(item.Link)
	}

	common.HTMLPost()
}
