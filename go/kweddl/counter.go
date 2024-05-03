package main

import (
	"log"
	"os"
	"strconv"
	"strings"
)

var _counter int
var _baseCounter int

func handleError(err error) {
	if err == nil {
		return
	}

	log.Fatal(err)
}

// CounterLoad - load the counter file
func CounterLoad() {
	_, err := os.Stat(CounterFile)
	if err != nil {
		_counter = 0
		return
	}

	content, err := os.ReadFile(CounterFile)
	handleError(err)

	_counter, err = strconv.Atoi(strings.TrimSpace(string(content)))
	handleError(err)

	_baseCounter = _counter
}

// CounterSeen - has this counter been seen before?
func CounterSeen(counter int) bool {
	return counter <= _baseCounter
}

// CounterMark - "mark" a counter as seen
func CounterMark(counter int) {
	if counter < _counter {
		return
	}

	_counter = counter
}

// CounterSave - save the counter file
func CounterSave() {
	cnt := strconv.Itoa(_counter)
	err := os.WriteFile(CounterFile, []byte(cnt), 0644)
	handleError(err)
}
