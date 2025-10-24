package main

import (
	"flag"
	"fmt"

	U "codeberg.org/lorentzus/tools/internal/usage"
)

var doc = `Usage: imgsize v1.0
imgsize [OPTIONS] [image-file1 image-file2 ... image-fileX]
[OPTIONS]
 -h              Help (this page)
`

func main() {
	help := flag.Bool("h", false, "Help")
	flag.Parse()
	if *help {
		U.Usage(doc, 0)
	}

	args := flag.Args()
	if len(args) == 0 {
		U.Usage(doc, 0)
	}

	for _, file := range args {
		width, height := GetSize(file)
		fmt.Printf("%s: %d / %d", file, width, height)
		fmt.Println()
	}
}
