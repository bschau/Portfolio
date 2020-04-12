package lib

import (
	"fmt"
	"io"
	"os"
)

var verbose bool

// LogVerbose - set verbosity
func LogVerbose(v bool) {
	verbose = v
}

// Debug - debug logging
func Debug(parts ...interface{}) {
	if verbose {
		output(os.Stdout, parts...)
	}
}

// Info - normal output
func Info(parts ...interface{}) {
	output(os.Stdout, parts...)
}

// Error - error logging
func Error(parts ...interface{}) {
	output(os.Stderr, parts...)
	os.Exit(1)
}

// Warn - warn logging
func Warn(parts ...interface{}) {
	output(os.Stderr, parts...)
}

func output(out io.Writer, parts ...interface{}) {
	if len(parts) < 1 {
		return
	}

	prefix := ""
	for _, part := range parts {
		fmt.Fprint(out, prefix, part)
		prefix = " "
	}
	fmt.Fprintln(out)
}
