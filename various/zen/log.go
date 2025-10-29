package main

import (
	"fmt"
	"io"
	"os"
)

// Verbose - indicate whether debug is logged
var Verbose bool

// Debug - debug logging
func Debug(parts ...interface{}) {
	if Verbose {
		output(os.Stdout, parts...)
	}
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
