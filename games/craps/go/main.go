package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/eiannone/keyboard"
)

var seededRand *rand.Rand

func main() {
	s1 := rand.NewSource(time.Now().UnixNano())
	seededRand = rand.New(s1)

	fmt.Println("Craps")
	fmt.Println("If you throw a 7 or 11 on your first throw, you win immediately. A score")
	fmt.Println("of 2, 3 or 12 on your first throw means you lose. If you throw anything")
	fmt.Println("else you get another throw.")
	fmt.Println("On subsequent throws, you win by matching your first throw, or lose by")
	fmt.Println("throwing a 7.")
	fmt.Println()
	fmt.Println("Press any key at the prompt '>' to continue ...")
	fmt.Println()
	engine()
	fmt.Println()
}

func engine() {
	round := 0
	previous := 0

	for {
		sum := throwDice() + throwDice()
		round++
		if round == 1 {
			if sum == 7 || sum == 11 {
				fmt.Println("You score", sum, "and win!")
				return
			}

			if sum == 2 || sum == 3 || sum == 12 {
				fmt.Println("You score", sum, "and lose :-(")
				return
			}

			previous = sum
		} else if sum == previous {
			fmt.Println("You score", sum, "and win by matching first throw :-)")
			return
		} else if sum == 7 {
			fmt.Println("You score 7 but not in first round, so you lose :-(")
			return
		}

		fmt.Println("You score", sum)
		fmt.Print(">")
		getKey()
		fmt.Println()
	}
}

func throwDice() int {
	return seededRand.Intn(6) + 1
}

func getKey() rune {
	char, _, err := keyboard.GetSingleKey()
	if err != nil {
		panic(err)
	}

	return char
}
