package lib

import (
	"fmt"
	"io"
	"os"
)

var name string
var usageText string

// UsageSetup - setup
func UsageSetup(programName string, usage string) {
	name = programName + " from Tools v" + Version
	usageText = "Usage: " + usage
}

// Usage - write usage and exit with exit code
func Usage(exitCode int) {
	s := getStream(exitCode)
	fmt.Fprint(s, name)
	fmt.Fprintln(s)
	fmt.Fprint(s, usageText)
	os.Exit(exitCode)
}

func getStream(exitCode int) io.Writer {
	if exitCode != 0 {
		return os.Stderr
	}

	return os.Stdout
}
