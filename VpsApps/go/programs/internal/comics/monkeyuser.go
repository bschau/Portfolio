package comics

import (
	"internal/common"
	"strings"
)

// MonkeyUser Handle the Monkey User comic
func MonkeyUser() {
	url := "https://www.monkeyuser.com"
	data := FetchURL(url)
	if data == nil {
		return
	}

	text := string(data)
	pos := strings.Index(text, "div class=\"content\">")
	pos = IndexAt(text, "src", pos)
	startPos := pos + 5
	endPos := IndexAt(text, "\"", startPos)
	link := text[startPos:endPos]
	title := "Monkey User"
	common.HTMLHeader(title, url)
	common.HTMLAddComic(title, link, "900")
	common.HTMLLineBreak("")
}
