package main

import (
	"flag"
	"fmt"
	"io"
	"math"
	"os"
	"strconv"
)

func main() {
	origoIsAmplitude := flag.Bool("0", false, "Origo is amplitude")
	entriesPrLine := flag.Int("e", 16, "Entries pr. line")
	help := flag.Bool("h", false, "Help")
	outputType := flag.String("o", "c", "Output type")
	tableLength := flag.Int("t", 256, "Table length")
	flag.Parse()

	if *help {
		usage(0)
	}

	args := flag.Args()
	if len(args) != 1 {
		usage(1)
	}

	amplitude, err := strconv.Atoi(args[0])
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	origo := 0
	if *origoIsAmplitude {
		origo = amplitude
	}

	cosTable := generateTable(amplitude, *tableLength, origo, math.Cos)
	sinTable := generateTable(amplitude, *tableLength, origo, math.Sin)

	if *outputType == "c" {
		COutput(*entriesPrLine, *tableLength, cosTable, sinTable)
	} else {
		fmt.Fprintln(os.Stderr, "Unknown output type:", *outputType)
		os.Exit(1)
	}
	os.Exit(0)
}

func usage(ec int) {
	doc := `Usage: waves v1.0
waves [OPTIONS] amplitude
[OPTIONS]
 -0              Origo is amplitude
 -e length       Entries pr. line, usually defaults to 256
 -h help         This page
 -o type         Output type (c)
 -t length       Table length, defaults to 256
`

	s := getStream(ec)
	fmt.Fprint(s, doc)
	os.Exit(ec)
}

func getStream(exitCode int) io.Writer {
	if exitCode != 0 {
		return os.Stderr
	}

	return os.Stdout
}

func generateTable(amplitude int, tableLength int, origo int, fp func(float64) float64) []int {
	step := (2 * math.Pi) / float64(tableLength)
	point := 0.0
	table := make([]int, tableLength)

	for i := 0; i < tableLength; i++ {
		value := int(fp(point)*float64(amplitude)) + origo
		table[i] = value
		point += step
	}

	return table
}
