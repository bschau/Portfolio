package main

import (
	"encoding/base64"
	"fmt"
	"strconv"
	"strings"
)

// CmdList - list stories
func CmdList(zen ZenFile, args []string, width int) {
	all := HasAll(args)
	brief := HasBrief(args)
	var builder strings.Builder

	for _, item := range zen.Items {
		if !shouldShow(item, all) {
			continue
		}

		decoded, _ := base64.StdEncoding.DecodeString(item.Text)
		story := string(decoded)
		if brief {
			builder.Reset()
			if !item.Open {
				builder.WriteString("*")
			}
			builder.WriteString(strconv.Itoa(item.ID))
			builder.WriteString(". ")
			builder.WriteString(strings.TrimSpace(story))
			fmt.Println(briefify(builder.String(), width))
			continue
		}

		fmt.Printf("ID: %d", item.ID)
		fmt.Println()
		fmt.Printf("Created:  %s", item.Created)
		fmt.Println()
		if item.Modified != item.Created {
			fmt.Printf("Modified: %s", item.Modified)
			fmt.Println()
		}

		if !item.Open {
			fmt.Printf("Closed:   %s", item.Closed)
			fmt.Println()
		}
		fmt.Printf("%s", story)
		fmt.Println()
		fmt.Println()
	}
}

func shouldShow(item ZenItem, all bool) bool {
	if all {
		return true
	}

	return item.Open
}

func briefify(source string, width int) string {
	newline := strings.Index(source, "\n")
	if newline > -1 {
		source = source[:newline]
	}

	if len(source) > width {
		source = source[:width]
	}

	return strings.TrimSpace(source)
}
