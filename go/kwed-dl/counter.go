package main

import (
	"io/ioutil"
	"log"
	"os"
	"strconv"
	"strings"
)

var counter int

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
		counter = 0
		return
	}

	content, err := ioutil.ReadFile(CounterFile)
	handleError(err)

	counter, err = strconv.Atoi(strings.TrimSpace(string(content)))
	handleError(err)
}

// CounterGet - get the counter (so java-ish)
func CounterGet() int {
	return counter
}

// CounterRecord - record new high
func CounterRecord(new int) {
	if new < counter {
		return
	}

	counter = new
	cnt := strconv.Itoa(counter)
	err := ioutil.WriteFile(CounterFile, []byte(cnt), 0644)
	handleError(err)
}
