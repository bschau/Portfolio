package main

import (
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"regexp"
)

// Traverse - traverse and search for files/folders matching pstterns
func Traverse(root string, term string) {
	regex := regexp.MustCompile(term)
	err := filepath.Walk(root, func(path string, info os.FileInfo, err error) error {
		if regex.MatchString(info.Name()) {
			fi, err := os.Lstat(path)
			if err != nil {
				fmt.Fprintln(os.Stderr, err)
				return nil
			}

			fsoType := " "
			mode := fi.Mode()
			if mode.IsRegular() {
				fsoType = "f"
			} else if mode.IsDir() {
				fsoType = "d"
			} else if mode&fs.ModeSymlink != 0 {
				fsoType = "l"
			} else if mode&fs.ModeNamedPipe != 0 {
				fsoType = "p"
			} else if mode&fs.ModeSocket != 0 {
				fsoType = "s"
			}

			fmt.Println(fsoType, path)
		}
		return nil
	})

	if err != nil {
		fmt.Fprintln(os.Stderr, err)
	}
}
