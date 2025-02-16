package main

import (
	"fmt"
	"io"
	"os"
)

// Usage writes the supplied text to stdout or stderr depending on errorcode (ec)
func Usage(text string, ec int) {
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
