package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

func main() {
	dryrun := flag.Bool("d", false, "Dry run - show what will be renamed")
	help := flag.Bool("h", false, "Help")
	quiet := flag.Bool("q", false, "Quiet mode")
	folders := flag.Bool("F", false, "Also rename folders")
	upper := flag.Bool("U", false, "Rename to uppercase")
	flag.Parse()

	if *help {
		usage(0)
	}

	DryRun = *dryrun
	Quiet = *quiet
	Folders = *folders
	Upper = *upper

	ToLower()

	os.Exit(0)
}

func usage(ec int) {
	doc := `Usage: tolower 1.0
tolower [OPTIONS]
[OPTIONS]
 -d              Dry run - show what would be renamed
 -F              Also rename folders
 -h              Help (this page)
 -q              Quiet mode
 -U              Rename to uppercase
`

	s := getStream(ec)
	fmt.Fprint(s, doc)
	os.Exit(ec)
}

func getStream(exitCode int) io.Writer {
	if exitCode != 0 {
		return os.Stderr
	}

	return os.Stdout
}
