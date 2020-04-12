package main

import (
	"flag"
	"os"
	"strconv"

	"../lib"
)

func main() {
	usageSetup()

	help := flag.Bool("h", false, "Help")
	flag.Parse()
	lib.LogVerbose(true)

	if *help {
		lib.Usage(0)
	}

	args := flag.Args()
	if len(args) == 0 {
		lib.Usage(1)
	}

	file := args[0]
	fileInfo, err := os.Stat(file)
	if err != nil {
		lib.Error(err)
	}

	f, err := os.Open(file)
	if err != nil {
		lib.Error(err)
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
			lib.Error(err)
		}
		s = v

		if len(args) > 2 {
			v, err := strconv.ParseInt(args[2], 10, 64)
			if err != nil {
				lib.Error(err)
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

func usageSetup() {
	doc := `hxd [OPTIONS] file [start [end]]
[OPTIONS]
 -h              Help (this page)
`
	lib.UsageSetup("hxd", doc)
}
