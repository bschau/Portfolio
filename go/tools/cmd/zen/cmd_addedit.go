package main

import (
	"encoding/base64"
	"io/ioutil"
	"strings"
)

// CmdAdd - add story
func CmdAdd(zen ZenFile, filename string, args []string, separator string) {
	if len(args) == 0 {
		Error("Missing argument(s) to 'add' command")
	}

	text := getStoryText(args, 0, separator)
	now := Now()
	item := ZenItem{
		ID:       zen.Next,
		Created:  now,
		Modified: now,
		Open:     true,
		Text:     text,
	}
	zen.Next++
	zen.Items = append(zen.Items, item)
	ZenFileSave(zen, filename)
}

// CmdEdit - edit a given story
func CmdEdit(zen ZenFile, filename string, args []string, separator string) {
	if len(args) < 2 {
		Error("Missing argument(s) to 'edit' command")
	}

	index := ZenItemIndexByID(zen, args[0])
	if !zen.Items[index].Open {
		Error("Story not open:", zen.Items[index].ID)
	}
	zen.Items[index].Modified = Now()
	zen.Items[index].Text = getStoryText(args, 1, separator)

	ZenFileSave(zen, filename)
}

func getStoryText(args []string, startIndex int, separator string) string {
	var builder strings.Builder

	prefix := ""
	for i := startIndex; i < len(args); i++ {
		if len(prefix) > 0 {
			builder.WriteString(prefix)
		}
		builder.WriteString(getText(args[i]))
		prefix = separator
	}

	return base64.StdEncoding.EncodeToString([]byte(builder.String()))
}

func getText(arg string) string {
	first := string(arg[0])
	if first == "\\" {
		if len(arg) > 1 {
			return arg[1:]
		}

		return "\\"
	}

	if first == "@" {
		content, err := ioutil.ReadFile(arg[1:])
		if err != nil {
			Error(err)
		}

		return string(content)
	}

	return arg
}
