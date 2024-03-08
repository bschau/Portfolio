package comics

import (
	"fmt"
	"internal/common"
	"strings"
	"time"
)

// Dilbert Handle the Dilbert comic
func Dilbert() {
	url := "https://dilbert.com"
	now := time.Now().Local()
	date := fmt.Sprintf("%02d-%02d-%02d", now.Year(), now.Month(), now.Day())

	remote := url + "/strip/" + date
	data := FetchURL(remote)
	if data == nil {
		return
	}

	text := string(data)
	pos := strings.Index(text, "img-responsive img-comic")
	pos = IndexAt(text, "src", pos)
	startPos := pos + 5
	endPos := IndexAt(text, "\"", startPos)
	link := text[startPos:endPos]
	common.HTMLHeader("Dilbert", url)
	common.HTMLAddComic("Dilbert", link, "")
	common.HTMLLineBreak("")
}
