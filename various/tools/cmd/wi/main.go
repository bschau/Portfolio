package main

import (
	"flag"
	"os"

	U "codeberg.org/lorentzus/tools/internal/usage"
)

var doc = `Usage: wi v1.0
wi [OPTIONS] term1 term2 ... termX
[OPTIONS]
 -h              Help (this page)
 -r root-folder  Folder to start search in

If root-folder is not given, default to . (current directory).
`

func main() {
	help := flag.Bool("h", false, "Help")
	rootFolder := flag.String("r", ".", "Root-folder to start search in")
	flag.Parse()

	if *help {
		U.Usage(doc, 0)
	}

	args := flag.Args()
	if len(args) == 0 {
		U.Usage(doc, 1)
	}

	for _, term := range args {
		Traverse(*rootFolder, term)
	}

	os.Exit(0)
}
