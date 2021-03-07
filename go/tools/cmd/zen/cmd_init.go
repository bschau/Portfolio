package main

import (
	"os"
)

// CmdInit - initialize a new zen file
func CmdInit(filename string) {
	ensureNoZenFile(filename)

	zen := ZenFile{
		Created: Now(),
		Next:    1,
		Items:   []ZenItem{},
	}

	ZenFileSave(zen, filename)
}

func ensureNoZenFile(filename string) {
	_, err := os.Stat(filename)
	if os.IsNotExist(err) {
		return
	}

	Error("A .zen file already exists here:", filename)
}
