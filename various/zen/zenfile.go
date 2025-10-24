package main

import (
	"encoding/json"
	"fmt"
	"os"
	"strconv"
)

// ZenItem is a single item
type ZenItem struct {
	ID       int
	Created  string
	Modified string
	Closed   string
	Open     bool
	Text     string
}

// ZenFile is the overall zen-file object
type ZenFile struct {
	Version  string
	Created  string
	Modified string
	Next     int
	Items    []ZenItem
}

// ZenFileMaster - get path of master file
func ZenFileMaster() string {
	path := os.Getenv("ZEN_FILE")
	if len(path) == 0 {
		path = os.Getenv("HOME") + "/.zen"
	}

	return path
}

// ZenFileLocate - locate the .zen file
func ZenFileLocate(master bool) string {
	if master {
		Debug("Looking for Masterfile")
		path := ZenFileMaster()

		info, err := os.Stat(path)
		if os.IsNotExist(err) {
			Debug("Masterfile not found:", path)
			os.Exit(1)
		}

		if info.IsDir() {
			Debug("Masterfile is a directory:", path)
			os.Exit(1)
		}

		return path
	}

	zenFile := ""
	previousFolder := ""

	for zenFile == "" {
		wd, err := os.Getwd()
		if err != nil {
			Error("Failed to get working directory")
		}

		if wd == previousFolder {
			Error("No .zen file found")
		}

		Debug("Looking for .zen in:", wd)

		stat, err := os.Stat(".zen")
		if os.IsNotExist(err) {
			previousFolder = wd
			err := os.Chdir("..")
			if err != nil {
				Error(err)
			}

			continue
		}

		if stat.IsDir() {
			Error(".zen is a directory in:", wd)
		}

		zenFile = wd + "/.zen"
	}

	return zenFile
}

// ZenFileLoad - load the zen file
func ZenFileLoad(filename string) ZenFile {
	file, err := os.Open(filename)
	if err != nil {
		Error(err)
	}

	defer file.Close()

	decoder := json.NewDecoder(file)
	zen := ZenFile{}
	err = decoder.Decode(&zen)
	if err != nil {
		Error(err)
	}

	if zen.Version != "ZEN1" {
		Error("Unknown zen-file version:", zen.Version)
	}

	return zen
}

// ZenFileSave - save the zenfile
func ZenFileSave(zen ZenFile, filename string) {
	backupFile(filename)

	file, err := os.Create(filename)
	if err != nil {
		Error(err)
	}

	defer file.Close()

	zen.Version = "ZEN1"
	zen.Modified = Now()

	jsonString, err := json.Marshal(zen)
	fmt.Fprintf(file, string(jsonString))
}

func backupFile(filename string) {
	_, err := os.Stat(filename)
	if os.IsNotExist(err) {
		return
	}

	err = os.Rename(filename, filename+"~")
	if err != nil {
		Error(err)
	}
}

// ZenItemIndexByID - get a given item by id
func ZenItemIndexByID(zen ZenFile, arg string) int {
	storyID, err := strconv.Atoi(arg)
	if err != nil {
		Error("Invalid story id:", arg)
	}

	for index, item := range zen.Items {
		if item.ID == storyID {
			return index
		}
	}

	Error("Story by ID not found:", storyID)
	return -1 // Shut-up, go
}
