package main

import (
	"fmt"
	"io"
	"os"
)

// Usage - write usage and exit with exit code
func Usage(exitCode int) {
	s := getStream(exitCode)
	doc := `eplogr v1.1
Usage: eplogr [OPTIONS] [destination-dir]

[OPTIONS]
 -c config       Configuration file (default is ~/[._]eplogrrc)
 -D              Disable file-writing
 -d domain       Ngrok domain
 -e extension    File-extension of received file
 -m size         Max size of request data shown / written to file (default is 10KB)
 -h              Help (this page)
 -t token        Authentication token

These can also be set in the environment (EPLOGR_DOMAIN, EPLOGR_AUTHTOKEN,
EPLOGR_EXTENSION, EPLOGR_DESTDIR, EPLOGR_MAXSIZE).
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
