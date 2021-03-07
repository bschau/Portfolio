package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

func main() {
	help := flag.Bool("h", false, "Help")
	rootFolder := flag.String("r", ".", "Root-folder to start search in")
	flag.Parse()

	if *help {
		usage(0)
	}

	args := flag.Args()
	if len(args) == 0 {
		usage(1)
	}

	for _, term := range args {
		Traverse(*rootFolder, term)
	}

	os.Exit(0)
}

func usage(ec int) {
	doc := `Usage: wi v1.0
wi [OPTIONS] term1 term2 ... termX
[OPTIONS]
 -h              Help (this page)
 -r root-folder  Folder to start search in

If root-folder is not given, default to . (current directory).
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
