package main

import (
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strconv"
	"strings"
)

// Rename - rename files in the specified folder with prefix
func Rename(folder string, prefix string, commit bool) {
	allFiles, err := os.ReadDir(folder)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	files := []string{}
	for _, file := range allFiles {
		fileName := file.Name()
		if strings.HasPrefix(fileName, ".") {
			continue
		}

		if strings.HasSuffix(fileName, "~") {
			continue
		}

		if !file.Type().IsRegular() {
			continue
		}

		files = append(files, fileName)
	}

	if len(files) < 1 {
		fmt.Println("No files to rename.")
		os.Exit(0)
	}

	if len(files) >= 10000000000 {
		fmt.Fprintln(os.Stderr, "Too many files!")
		os.Exit(1)
	}

	sort.Strings(files)
	formatter := prefix + "-" + getFormatString(len(files)) + "%s"
	index := 1
	dstName := ""
	for _, file := range files {
		src := filepath.Join(folder, file)
		index, dstName = getNewName(folder, formatter, file, index)
		fmt.Println(src, " => ", dstName)

		if commit {
			dst := filepath.Join(folder, dstName)
			if err := os.Rename(src, dst); err != nil {
				fmt.Fprintln(os.Stderr, err)
			}
		}
		index++
	}

	if !commit {
		fmt.Println()
		fmt.Println("Changes not commited. Run with 'commit' as last argument to commit changes.")
	}
}

func getFormatString(cnt int) string {
	str := strconv.Itoa(cnt)
	len := len(str)
	if len == 1 {
		return "%d"
	}

	return "%0" + strconv.Itoa(len) + "d"
}

func getNewName(folder string, formatter string, file string, index int) (int, string) {
	ext := filepath.Ext(file)
	if len(ext) > 0 {
		ext = strings.ToLower(ext)
	}

	dstName := ""
	for {
		dstName = fmt.Sprintf(formatter, index, ext)
		if _, err := os.Stat(filepath.Join(folder, dstName)); err != nil {
			break
		}
		lastIndex := index
		index++
		if lastIndex > index {
			fmt.Fprintln(os.Stderr, "Overflow - too many files!")
			os.Exit(1)
		}
	}

	return index, dstName
}
