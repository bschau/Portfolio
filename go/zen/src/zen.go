package main

import (
	"flag"
	"fmt"
	"os"
	"strings"
	"time"
)

func main() {
	if len(os.Args) == 0 {
		Usage(0)
	}

	file := flag.String("f", "", "Path to zen-file")
	help := flag.Bool("h", false, "Help")
	separator := flag.String("s", " ", "Separator string")
	verbose := flag.Bool("v", false, "Verbose mode")
	width := flag.Int("w", 80, "Width of display to 'list brief' command")
	flag.Parse()

	if *help {
		Usage(0)
	}

	args := flag.Args()
	if len(args) == 0 {
		Usage(1)
	}

	Verbose = *verbose

	index := 0
	master := false
	if strings.ToLower(args[index]) == "master" ||
		strings.ToLower(args[index]) == "m" {
		master = true
		index++
	}

	if len(args) < index+1 {
		Usage(1)
	}

	command := strings.ToLower(args[index])
	if command == "help" {
		Usage(0)
	}

	if command == "init" {
		path := ".zen"
		if len(*file) > 0 {
			path = *file
		} else if master {
			path = ZenFileMaster()
		}
		CmdInit(path)
		os.Exit(0)
	}

	zenFile := *file
	if len(zenFile) < 1 {
		zenFile = ZenFileLocate(master)
	}

	if len(zenFile) < 1 {
		fmt.Fprintf(os.Stderr, "Cannot find zen-file\n")
		os.Exit(1)
	}

	zen := ZenFileLoad(zenFile)

	newArgIndex := 1
	if master {
		newArgIndex++
	}

	newArgs := args[newArgIndex:]

	if command == "add" || command == "a" {
		CmdAdd(zen, zenFile, newArgs, *separator)
	} else if command == "close" || command == "cl" {
		CmdClose(zen, zenFile, newArgs)
	} else if command == "count" || command == "c" {
		CmdCount(zen, newArgs)
	} else if command == "count0" || command == "c0" {
		CmdCount0(zen, newArgs)
	} else if command == "delete" || command == "d" {
		CmdDelete(zen, zenFile, newArgs)
	} else if command == "edit" || command == "e" {
		CmdEdit(zen, zenFile, newArgs, *separator)
	} else if command == "list" || command == "l" {
		if *width < 1 {
			Error("Illegal width:", *width)
		}
		CmdList(zen, newArgs, *width)
	} else if command == "reopen" || command == "r" {
		CmdReopen(zen, zenFile, newArgs)
	} else if command == "status" || command == "s" {
		CmdStatus(zen, newArgs)
	} else if command == "view" || command == "v" {
		CmdView(zen, newArgs)
	}
}

// Now - get current time
func Now() string {
	return time.Now().Format(time.RFC3339)
}

// HasAll - is "all" / "a" part of arguments list
func HasAll(args []string) bool {
	return search(args, "all", "a")
}

// HasBrief - is "brief" / "b" part of arguments list
func HasBrief(args []string) bool {
	return search(args, "brief", "b")
}

func search(args []string, long string, short string) bool {
	if len(args) == 0 {
		return false
	}

	for _, arg := range args {
		if strings.ToLower(arg) == long ||
			strings.ToLower(arg) == short {
			return true
		}
	}

	return false
}
