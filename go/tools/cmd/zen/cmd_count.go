package main

import (
	"fmt"
)

// CmdCount - count number of stories
func CmdCount(zen ZenFile, args []string) {
	count := getCount(zen, args)
	fmt.Println(count)
}

// CmdCount0 - count number of stories
func CmdCount0(zen ZenFile, args []string) {
	count := getCount(zen, args)
	fmt.Print(count)
}

func getCount(zen ZenFile, args []string) int {
	all := HasAll(args)
	if all {
		return len(zen.Items)
	}

	count := 0
	for _, i := range zen.Items {
		if i.Open {
			count++
		}
	}

	return count
}
