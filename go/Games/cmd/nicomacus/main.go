package main

import (
	"fmt"
	"math/rand"
	"time"
)

var seededRand *rand.Rand

func main() {
	s1 := rand.NewSource(time.Now().UnixNano())
	seededRand = rand.New(s1)

	fmt.Println("Nicomacus")
	fmt.Println("=========")
	nicomacus()
	fmt.Println()
}

func nicomacus() {
	for {
		fmt.Println()
		fmt.Println("Think of a number between 1 and 100.")
		fmt.Println("Press CTRL + C to stop.\n")
		rem3 := getRemainder("Your number divided by 3 has a remainder of")
		rem5 := getRemainder("Your number divided by 5 has a remainder of")
		rem7 := getRemainder("Your number divided by 7 has a remainder of")

		result := calculateNumber(rem3, rem5, rem7)
		fmt.Println("Your number was", result, "...")
	}
}

func getRemainder(prompt string) int {
	for {
		fmt.Println(prompt)
		fmt.Print("> ")

		var remainder int
		n, err := fmt.Scanf("%d", &remainder)
		if n != 1 {
			fmt.Println(err)
			continue
		}

		return remainder
	}
}

func calculateNumber(rem3 int, rem5 int, rem7 int) int {
	d := (70 * rem3) + (21 * rem5) + (15 * rem7)

	for d > 105 {
		d -= 105
	}

	return d
}
