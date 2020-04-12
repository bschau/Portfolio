package main

import (
	"os"
	"path/filepath"
	"strings"

	"../lib"
)

// DryRun - traverse but don't delete
var DryRun bool

// IgnoreDotFiles - ignore dot files (.rc, .something)
var IgnoreDotFiles bool

// IgnoreUnderscoreFiles - ignore underscore files (_rc, _something)
var IgnoreUnderscoreFiles bool

// TraceFiles - print files seen
var TraceFiles bool

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
			lib.Info(path)
		}

		if !strings.HasSuffix(path, "~") {
			return nil
		}

		name := info.Name()
		if IgnoreDotFiles && strings.HasPrefix(name, ".") {
			lib.Debug("Ignored by user:", path)
			return nil
		}

		if IgnoreUnderscoreFiles && strings.HasPrefix(name, "_") {
			lib.Debug("Ignored by user:", path)
			return nil
		}

		if DryRun {
			return nil
		}

		files = append(files, path)
		return nil
	})

	if err != nil {
		lib.Warn(err)
		return
	}

	for _, file := range files {
		lib.Debug(file)
		err := os.Remove(file)
		if err != nil {
			lib.Warn(err, file)
		}
	}
}

func getWd() string {
	dir, err := os.Getwd()
	if err != nil {
		lib.Error(err)
	}

	return dir
}
