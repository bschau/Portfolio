package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
)

func main() {
	help := flag.Bool("h", false, "Help")
	flag.Parse()

	if *help {
		usage(0)
	}

	args := flag.Args()
	if len(args) == 0 {
		usage(1)
	}

	file := args[0]
	fileInfo, err := os.Stat(file)
	if err != nil {
		log.Fatal(err)
	}

	f, err := os.Open(file)
	if err != nil {
		log.Fatal(err)
	}

	defer f.Close()

	var s int64
	var e int64

	fileSize := fileInfo.Size()
	s = 0
	e = fileSize
	if len(args) > 1 {
		v, err := strconv.ParseInt(args[1], 10, 64)
		if err != nil {
			log.Fatal(err)
		}
		s = v

		if len(args) > 2 {
			v, err := strconv.ParseInt(args[2], 10, 64)
			if err != nil {
				log.Fatal(err)
			}
			e = v
		}

		if s > e {
			t := e
			e = s
			s = t
		}
		e++
		if e > fileSize {
			e = fileSize
		}
	}

	HexDump(f, s, e)
	os.Exit(0)
}

func usage(ec int) {
	doc := `Usage: hxd v1.0
hxd [OPTIONS] file [start [end]]
[OPTIONS]
 -h              Help (this page)
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
