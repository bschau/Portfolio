package main

import (
	"fmt"
)

// CmdStatus - view status of story / stories
func CmdStatus(zen ZenFile, args []string) {
	argLength := len(args)
	if argLength == 0 {
		Error("Missing argument(s) to 'status' command")
	}

	prefix := ""
	for i := 0; i < argLength; i++ {
		index := ZenItemIndexByID(zen, args[i])
		item := zen.Items[index]
		fmt.Print(prefix)

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

		prefix = "\n"
	}
}
