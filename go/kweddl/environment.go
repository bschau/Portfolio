package main

import (
	"os"
	"runtime"
)

// CounterFile - path to counter file
var CounterFile string

// Desktop - path to users desktop
var Desktop string

// EnvironmentInit - setup the working environment
func EnvironmentInit() {
	var home string
	var prefix string
	if runtime.GOOS == "windows" {
		home = os.Getenv("HOMEDRIVE") + os.Getenv("HOMEPATH")
		prefix = "_"
	} else {
		home = os.Getenv("HOME")
		prefix = "."
	}

	CounterFile = home + "/" + prefix + "kwedrc"
	Desktop = home + "/Desktop"
}
