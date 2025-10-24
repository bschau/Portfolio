package main

import (
	"flag"
	"fmt"
	"log"
	"math/rand"
	"os"
	"strings"
	"strconv"
	"time"

	U "codeberg.org/lorentzus/tools/internal/usage"
)

var doc = `Usage: genpwd v1.0
genpwd [OPTIONS] [number of passwords]
[OPTIONS]
 -d digits       Number of digits in passwords
 -h              Help (this page)
 -l length       Length of passwords
`

func main() {
	help := flag.Bool("h", false, "Help")
	digits := flag.Int("d", 2, "Number of digits in a password")
	length := flag.Int("l", 20, "Length of a password")
	flag.Parse()

	if *help {
		U.Usage(doc, 0)
	}

	if *digits >= *length {
		log.Fatal("Number of digits in a password must be lower than the password length")
	}

	args := flag.Args()
	if len(args) > 1 {
		U.Usage(doc, 1)
	}

	noOfPwds := getNumberOfPasswords(args)
	generate(noOfPwds, *length, *digits)
	os.Exit(0)
}

func getNumberOfPasswords(args []string) int {
	if len(args) == 0 {
		return 10
	}

	noOfPwds, err := strconv.Atoi(args[0])
	if err != nil {
		log.Fatal(err)
	}

	if noOfPwds < 1 {
		log.Fatal("You must generate at least one password")
	}

	return noOfPwds
}

func generate(noOfPwds int, length int, digits int) {
	const consonants string = "bcdfghjklmnpqrstvwxyz"
	const vocals string = "aeiou"
	const numbers string = "0123456789"
	length -= digits
	cLen := len(consonants)
	vLen := len(vocals)
	nLen := len(numbers)

	var sb strings.Builder
	s1 := rand.NewSource(time.Now().UnixNano())
	seededRand := rand.New(s1)

	for ; noOfPwds > 0; noOfPwds-- {
		sb.Reset()
		alphas := length / 2
		for i := 0; i < alphas; i++ {
			sb.WriteByte(consonants[seededRand.Intn(cLen)])
			sb.WriteByte(vocals[seededRand.Intn(vLen)])
		}

		if length % 2 == 1 {
			sb.WriteByte(vocals[seededRand.Intn(vLen)])
		}

		for i := 0; i < digits; i++ {
			sb.WriteByte(numbers[seededRand.Intn(nLen)])
		}

		fmt.Println(sb.String())
	}
}
