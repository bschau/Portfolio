package main

import (
	"flag"
	"math"
	"os"
	"strconv"

	"../lib"
)

func main() {
	usageSetup()

	origoIsAmplitude := flag.Bool("0", false, "Origo is amplitude")
	entriesPrLine := flag.Int("e", 16, "Entries pr. line")
	help := flag.Bool("h", false, "Help")
	outputType := flag.String("o", "c", "Output type")
	tableLength := flag.Int("t", 256, "Table length")
	flag.Parse()
	lib.LogVerbose(true)

	if *help {
		lib.Usage(0)
	}

	args := flag.Args()
	if len(args) != 1 {
		lib.Usage(1)
	}

	amplitude, err := strconv.Atoi(args[0])
	if err != nil {
		lib.Error(err)
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
		lib.Error("Unknown output type:", outputType)
	}
	os.Exit(0)
}

func usageSetup() {
	doc := `waves [OPTIONS] amplitude
[OPTIONS]
 -0              Origo is amplitude
 -e length       Entries pr. line, usually defaults to 256
 -h help         This page
 -o type         Output type (c)
 -t length       Table length, defaults to 256
`
	lib.UsageSetup("waves", doc)
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
