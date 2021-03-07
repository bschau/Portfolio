package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

func main() {
	dryrun := flag.Bool("d", false, "Dry run - show what will be deleted")
	help := flag.Bool("h", false, "Help")
	ignoreDotFiles := flag.Bool("i", false, "Ignore dot-files (.rc, .something, ...)")
	traceFiles := flag.Bool("t", false, "Trace files")
	ignoreUnderscoreFiles := flag.Bool("u", false, "Ignore underscore-files (_rc, _something, ...)")
	verbose := flag.Bool("v", false, "Verbose mode")
	flag.Parse()

	if *help {
		usage(0)
	}

	DryRun = *dryrun
	IgnoreDotFiles = *ignoreDotFiles
	IgnoreUnderscoreFiles = *ignoreUnderscoreFiles
	TraceFiles = *traceFiles
	Verbose = *verbose

	args := flag.Args()
	if len(args) == 0 {
		Traverse(".")
		os.Exit(0)
	}

	for _, file := range args {
		Traverse(file)
	}

	os.Exit(0)
}

func usage(ec int) {
	doc := `Usage: remtilde v1.0
remtilde [OPTIONS] [path1 path2 ... pathX]
[OPTIONS]
 -d              Dry run - show what would be deleted
 -h              Help (this page)
 -i              Ignore dot-files (.rc, .something, ...)
 -t              Trace files
 -u              Ignore underscore-files (_rc, _something, ...)
 -v              Verbose/Debug output

If paths are not given, default to . (current directory).
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
