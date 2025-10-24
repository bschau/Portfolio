package main

import (
	"flag"
	"fmt"
	"strings"

	U "codeberg.org/lorentzus/tools/internal/usage"
)

var doc = `Usage: colconv 1.0
colconv -h | xxxxxx | r g b | r-gl g-gl b-gl
[OPTIONS]
 -h              Help (this page)
 xxxxxx          Hex code
 r g b           Red, green and blue components in integer form
 r-gl g-gl b-gl  Red, green and blue components in Open GL form (0.xx)
`

func main() {
	help := flag.Bool("h", false, "Help")
	flag.Parse()
	if *help {
		U.Usage(doc, 0)
	}

	args := flag.Args()
	if len(args) < 1 || len(args) == 2 || len(args) > 3 {
		U.Usage(doc, 0)
	}

	var v1 int64 = 0
	var v2 int64 = 0
	var v3 int64 = 0
	if len(args) == 1 {
		v1, v2, v3 = ConvertFromHex(args[0])
	} else {
		if strings.Contains(args[0], ".") {
			v1, v2, v3 = ConvertFromOpenGL(args[0], args[1], args[2])
		} else {
			v1, v2, v3 = ConvertFromInt(args[0], args[1], args[2])
		}
	}

	fmt.Printf("Hexcolor:   #%02X%02X%02X", v1, v2, v3)
	fmt.Println()
	fmt.Printf("RGB:        %d, %d, %d", v1, v2, v3)
	fmt.Println()
	fmt.Printf("OpenGL ES:  %.2f, %.2f, %.2f", float64(v1)/255.0, float64(v2)/255.0, float64(v3)/255.0)
	fmt.Println()
}
