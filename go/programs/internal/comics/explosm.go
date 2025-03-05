package comics

import (
	"internal/common"
	"strings"
)

// Explosm Handle Explosm comic
func Explosm() {
	url := "http://explosm.net"
	data := FetchURL(url)
	if data == nil {
		return
	}

	common.HTMLHeader("Explosm", url)
	text := string(data)
	pos := strings.Index(text, "div id=\"comic-wrap\"")
	if pos >= 0 {
		pos = IndexAt(text, "flex-video", pos)
		if pos >= 0 {
			body := "<p>Today is a video. Click <a href=\"" + url + "\">here</a> to see it :-)</p>"
			common.HTMLLineBreak(body)
			return
		}
	}

	pos = strings.Index(text, "img id=\"main-comic\" src")
	startPos := pos + 25
	endPos := IndexAt(text, "\"", startPos)
	link := text[startPos:endPos]
	if link[:2] == "//" {
		link = "http:" + link
	}

	common.HTMLAddComic("Explosm", link, "900")
	common.HTMLLineBreak("")
}
