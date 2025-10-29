package main

import (
	"fmt"
	"log"
	"os"
	"strings"
)

// DryRun - traverse but don't rename 
var DryRun bool

// Folders - rename also folders
var Folders bool

// Quiet - do not output renamed file names
var Quiet bool

// Upper - rename to uppercase
var Upper bool

// DotFiles - include dotfiles and folders
var DotFiles bool

// FnChg - change case on files/folders in the current folder
func FnChg() {
	objs, err := os.ReadDir(".")

	if err != nil {
		log.Fatal(err)
	}

	for _, obj := range objs {
		name := obj.Name()
		if name == "." || name == ".." {
			continue
		}

		if !DotFiles && strings.HasPrefix(name, ".") {
			continue
		}

		if !Folders && obj.IsDir() {
			continue
		}

		dst := getNewName(name)
		if strings.Compare(name, dst) == 0 {
			continue
		}

		if !Quiet {
			fmt.Println(name, "->", dst)
		}

		if DryRun {
			continue
		}

		err := os.Rename(name, dst)
		if err != nil {
			log.Fatal(err)
		}
	}
}

func getNewName(name string) string {
	if Upper {
		return strings.ToUpper(name)
	}

	return strings.ToLower(name)
}
