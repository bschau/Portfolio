package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"strconv"
	"strings"
)

var builder strings.Builder

// COutput - output tables suitable for c
func COutput(entriesPrLine int, tableLength int, cosTable []int, sinTable []int) {
	builder.WriteString("#ifndef WAVE_TABLES_H\n#define WAVE_TABLES_H\n")
	output("cos_wave", entriesPrLine, tableLength, cosTable)
	output("sin_wave", entriesPrLine, tableLength, sinTable)
	builder.WriteString("\n#endif\n")

	err := ioutil.WriteFile("wave_tables.h", []byte(builder.String()), 0644)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}

func output(name string, entriesPrLine int, tableLength int, table []int) {
	builder.WriteString("\nstatic int ")
	builder.WriteString(name)
	builder.WriteString("[")
	builder.WriteString(strconv.Itoa(tableLength))
	builder.WriteString("] = {\n\t")

	entry := 0
	for i := 0; i < tableLength; i++ {
		builder.WriteString(strconv.Itoa(table[i]))
		entry++
		if entry == entriesPrLine {
			entry = 0
			if i == tableLength-1 {
				builder.WriteString("\n")
			} else {
				builder.WriteString(",\n\t")
			}
		} else {
			if i < tableLength-1 {
				builder.WriteString(", ")
			} else {
				builder.WriteString("\n")
			}
		}
	}

	builder.WriteString("};\n")
}
