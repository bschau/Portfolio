package comics

import (
	"internal/common"
	"strings"
)

// Generic Fetch Creators or King Features comic
func Generic(origin string, token string, title string, url string) {
	data := FetchURL(url)
	if data == nil {
		return
	}

	text := string(data)
	pos := getStartPosition(text, origin)
	pos = IndexAt(text, token, pos)
	startPos := pos + len(token) + 2
	endPos := IndexAt(text, "\"", startPos)
	link := text[startPos:endPos]
	common.HTMLHeader(title, url)
	common.HTMLAddComic(title, link, "900")
	common.HTMLLineBreak("")
}

func getStartPosition(text string, origin string) int {
	if len(origin) == 0 {
		return 0
	}

	return strings.Index(text, origin)
}
