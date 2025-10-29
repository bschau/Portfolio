package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

// DryRun - traverse but don't delete
var DryRun bool

// IgnoreDotFiles - ignore dot files (.rc, .something)
var IgnoreDotFiles bool

// IgnoreUnderscoreFiles - ignore underscore files (_rc, _something)
var IgnoreUnderscoreFiles bool

// TraceFiles - print files seen
var TraceFiles bool

// Verbose - raise verbosity
var Verbose bool

// Traverse - traverse path, deleting ~ files as we go
func Traverse(root string) {
	if root == "." {
		root = getWd()
	}

	var files []string
	err := filepath.Walk(root, func(path string, info os.FileInfo, err error) error {
		if info.IsDir() {
			return nil
		}

		if TraceFiles {
			fmt.Println(path)
		}

		if !strings.HasSuffix(path, "~") {
			return nil
		}

		name := info.Name()
		if IgnoreDotFiles && strings.HasPrefix(name, ".") {
			if Verbose {
				fmt.Println("Ignored by user:", path)
			}
			return nil
		}

		if IgnoreUnderscoreFiles && strings.HasPrefix(name, "_") {
			if Verbose {
				fmt.Println("Ignored by user:", path)
			}
			return nil
		}

		if DryRun {
			return nil
		}

		files = append(files, path)
		return nil
	})

	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return
	}

	for _, file := range files {
		if Verbose {
			fmt.Println(file)
		}
		err := os.Remove(file)
		if err != nil {
			fmt.Fprintln(os.Stderr, err, ": ", file)
		}
	}
}

func getWd() string {
	dir, err := os.Getwd()
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	return dir
}
