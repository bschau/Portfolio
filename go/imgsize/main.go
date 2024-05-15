package main

import (
	"flag"
	"fmt"
	"os"
	"io"
)

var doc = `Usage: imgsize v1.0
imgsize [OPTIONS] [image-file1 image-file2 ... image-fileX]
[OPTIONS]
 -h              Help (this page)
`

func usage(text string, ec int) {
	s := getStream(ec)
	fmt.Fprint(s, text)
	os.Exit(ec)
}

func getStream(exitCode int) io.Writer {
	if exitCode != 0 {
		return os.Stderr
	}

	return os.Stdout
}

func main() {
	help := flag.Bool("h", false, "Help")
	flag.Parse()
	if *help {
		usage(doc, 0)
	}

	args := flag.Args()
	if len(args) == 0 {
		usage(doc, 0)
	}

	for _, file := range args {
		width, height := GetSize(file)
		fmt.Printf("%s: %d / %d", file, width, height)
		fmt.Println()
	}
}
