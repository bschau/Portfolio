package main

import (
	"fmt"
	"io"
	"os"
)

// Usage - write usage and exit with exit code
func Usage(exitCode int) {
	s := getStream(exitCode)
	doc := `gxdavr v1.0
Usage: gxdavr [OPTIONS]

[OPTIONS]
 -c config       Configuration file (default is ~/.gxdavrrc)
 -h              Help (this page)
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
