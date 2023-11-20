package main

import (
	"fmt"
	"os"
	"path/filepath"
	"regexp"
)

// Traverse - traverse and search for files/folders matching pstterns
func Traverse(root string, term string) {
	regex := regexp.MustCompile(term)
	err := filepath.Walk(root, func(path string, info os.FileInfo, err error) error {
		if regex.MatchString(info.Name()) {
			fmt.Println(path)
		}
		return nil
	})

	if err != nil {
		fmt.Fprintln(os.Stderr, err)
	}
}
