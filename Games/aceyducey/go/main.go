package main

import (
	"fmt"
	"math/rand"
	"strconv"
	"time"
)

const (
	Clubs = iota
	Spades
	Hearts
	Diamonds
)
const NumCards = 52
const InitialMoney = 100

var cardNames = []string{"unused", "unused", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"}
var cardSuites = []string{"clubs", "spades", "hearts", "diamonds"}
var cards []int = make([]int, NumCards)
var seededRand *rand.Rand

func main() {
	s1 := rand.NewSource(time.Now().UnixNano())
	seededRand = rand.New(s1)

	fmt.Println("Acey Ducy")
	fmt.Println("=========")
	fmt.Println()
	fmt.Println("Two cards are drawn and shown face up.")
	fmt.Println("You bet on whether the third card drawn falls between the two cards.")
	fmt.Println("Enter 0 as amount to skip betting.")
	fmt.Println("If you win your bet is doubled as a price.")
	fmt.Println("The game will stop when there are no more cards to draw")
	fmt.Println("or when you run out of money.")
	fmt.Println()
	aceyducey()
	fmt.Println()
}

func aceyducey() {
	initCards()

	index := 0
	round := 0
	money := InitialMoney

	for {
		if money <= 0 {
			break
		}

		cardsLeft := NumCards - index
		if cardsLeft < 3 {
			break
		}

		round++
		fmt.Println("Round", round)
		fmt.Println("You have", fmtDollars(money, ""), "in your pocket")
		fmt.Println("The two cards:")
		card1 := cards[index]
		index++
		card2 := cards[index]
		index++
		fmt.Println("   ", getCardName(card1))
		fmt.Println("   ", getCardName(card2))
		bet := getBet(money)
		if bet == 0 {
			continue
		}

		money -= bet
		player := cards[index]
		index++
		fmt.Println("Your card:")
		fmt.Println("   ", getCardName(player))

		if didPlayerWin(card1, card2, player) {
			fmt.Println("You won", fmtDollars(bet, "!"))
			money += bet + bet
		} else {
			fmt.Println("You lost", fmtDollars(bet, "."), "Better luck next round!")
		}

		fmt.Println()
	}

	fmt.Println()
	fmt.Println("Game over.")
	if money > InitialMoney {
		fmt.Println("You came out with", fmtDollars(money-InitialMoney, ""), "more than you entered with!!")
	} else if money == InitialMoney {
		fmt.Println("You win some, you lose some. Nothing gained or lost.")
	} else {
		fmt.Println("Bummer! You lost", fmtDollars(InitialMoney-money, "."))
	}
}

func initCards() {
	initSuite(Clubs)
	initSuite(Spades)
	initSuite(Hearts)
	initSuite(Diamonds)

	fmt.Println("Shuffling cards ...")
	for i := 0; i < 1000; i++ {
		l := seededRand.Intn(NumCards)
		r := seededRand.Intn(NumCards)
		if l == r {
			continue
		}

		t := cards[l]
		cards[l] = cards[r]
		cards[r] = t
	}
}

func initSuite(suite int) {
	index := suite * 13
	suite <<= 4

	for c := 2; c < 15; c++ {
		cards[index] = c | suite
		index++
	}
}

func getCardName(c int) string {
	return fmt.Sprintf("%s of %s", cardNames[c&0xf], cardSuites[c>>4])
}

func fmtDollars(amount int, trail string) string {
	return fmt.Sprintf("$%s%s", strconv.Itoa(amount), trail)
}

func getBet(maxAmount int) int {
	for {
		fmt.Println("How much du you want to bet?")
		fmt.Print("> ")

		var bet int
		n, err := fmt.Scanf("%d", &bet)
		if n != 1 {
			fmt.Println(err)
			continue
		}

		if bet < 0 {
			fmt.Println("No can do.")
			continue
		}

		if bet == 0 {
			fmt.Println("Chicken!!!")
			return 0
		}

		if bet > maxAmount {
			fmt.Println("You only have", fmtDollars(maxAmount, ""), "to bet.")
			continue
		}

		return bet
	}
}

func didPlayerWin(card1 int, card2 int, player int) bool {
	card1 &= 0xf
	card2 &= 0xf
	player &= 0xf

	if card1 > card2 {
		c := card1
		card1 = card2
		card2 = c
	}

	return player > card1 && player < card2
}
