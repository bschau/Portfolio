package main

import (
	"encoding/base64"
	"fmt"
	"strings"
)

// CmdView - view a given story / stories
func CmdView(zen ZenFile, args []string) {
	argLength := len(args)
	if argLength == 0 {
		Error("Missing argument(s) to 'view' command")
	}

	prefix := ""
	for i := 0; i < argLength; i++ {
		index := ZenItemIndexByID(zen, args[i])
		item := zen.Items[index]
		fmt.Print(prefix)
		prefix = "\n"
		if !item.Open {
			Warn("Story not open:", item.ID)
			continue
		}

		if argLength > 2 {
			fmt.Printf("ID: %d", item.ID)
			fmt.Println()
		}

		decoded, _ := base64.StdEncoding.DecodeString(item.Text)
		story := strings.TrimSpace(string(decoded))
		fmt.Println(story)
	}
}
