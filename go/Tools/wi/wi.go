package main

import (
	"flag"
	"os"

	"../lib"
)

func main() {
	usageSetup()

	help := flag.Bool("h", false, "Help")
	rootFolder := flag.String("r", ".", "Root-folder to start search in")
	flag.Parse()
	lib.LogVerbose(true)

	if *help {
		lib.Usage(0)
	}

	args := flag.Args()
	if len(args) == 0 {
		lib.Usage(1)
	}

	for _, term := range args {
		Traverse(*rootFolder, term)
	}

	os.Exit(0)
}

func usageSetup() {
	doc := `wi [OPTIONS] term1 term2 ... termX
[OPTIONS]
 -h              Help (this page)
 -r root-folder  Folder to start search in

If root-folder is not given, default to . (current directory).
`
	lib.UsageSetup("wi", doc)
}
