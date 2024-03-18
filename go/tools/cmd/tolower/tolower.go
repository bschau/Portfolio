package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"
)

// DryRun - traverse but don't delete
var DryRun bool

// Folders - rename also folders
var Folders bool

// Quiet - do not output renamed file names
var Quiet bool

// Upper - rename to uppercase
var Upper bool

// ToLower - rename (to lowercase) files in the current folder
func ToLower() {
	err := filepath.Walk(".", func(path string, info os.FileInfo, err error) error {
		if !Folders && info.IsDir() {
			return nil
		}

		name := info.Name()
		if name == "." || name == ".." {
			return nil
		}

		dst := getNewName(name)
		if strings.Compare(name, dst) == 0 {
			return nil
		}

		if !Quiet {
			fmt.Println(name, "->", dst)
		}

		if DryRun {
			return nil
		}

		err = os.Rename(name, dst)
		if err != nil {
			log.Fatal(err)
			return err
		}

		return nil
	})

	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return
	}
}

func getNewName(name string) string {
	if Upper {
		return strings.ToUpper(name)
	}

	return strings.ToLower(name)
}
