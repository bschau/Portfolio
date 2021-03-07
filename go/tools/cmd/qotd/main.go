package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

func main() {
	help := flag.Bool("h", false, "Help")
	powerShell := flag.Bool("p", false, "Setup for PowerShell")
	flag.Parse()

	if *help {
		usage(0)
	}

	args := flag.Args()
	if len(args) > 1 {
		usage(1)
	}

	if len(args) == 1 {
		_, err := os.Stat(args[0])
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}

		showQotd(args[0], *powerShell)
		os.Exit(0)
	}

	qotdFile := QotdFind()
	showQotd(qotdFile, *powerShell)

	os.Exit(0)
}

func usage(exitCode int) {
	s := getStream(exitCode)
	fmt.Fprint(s, "qotd v1.0")
	fmt.Fprintln(s)
	doc := `qotd [OPTIONS] [qotd-file]
[OPTIONS]
 -h              Help (this page)
 -p              Setup for PowerShell

The qotd file used is located in this order:

1) Given as argument on the command-line.
2) Pointed to by QOTD_FILE environment variable.
3) Searched for in home folder (.qotd.txt / _qotd.txt).
`
	fmt.Fprint(s, doc)
	os.Exit(exitCode)
}

func getStream(exitCode int) io.Writer {
	if exitCode != 0 {
		return os.Stderr
	}

	return os.Stdout
}

func showQotd(qotdFile string, powerShell bool) {
	QotdLoad(qotdFile)
	qotd := QotdPick()
	QotdOutput(qotd, powerShell)
	fmt.Println()
}
