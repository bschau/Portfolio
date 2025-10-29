package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/eiannone/keyboard"
)

var coins int
var strength int
var seededRand *rand.Rand

func main() {
	s1 := rand.NewSource(time.Now().UnixNano())
	seededRand = rand.New(s1)

	fmt.Println("Adventure")
	fmt.Println("Welcome to the dark caves ... prepare to die!")
	fmt.Println()
	fmt.Println("Press any key at the prompt '>' to continue ...")
	fmt.Println()
	adventure()
	fmt.Println()
}

func adventure() {
	turn := 1
	coins = 0
	strength = 500

	for {
		fmt.Println("Turn:", turn, "Coins:", coins, "Strength:", strength)
		if strength <= 0 {
			fmt.Println("So long - nice knowing you. R.I.P.")
			return
		}

		switch random(4) {
		case 0:
			foundPotion()
		case 1:
			foundTreasure()
		case 2:
			foundTrap()
		case 3:
			foundMonster()
		}

		fmt.Print(">")
		getKey()
		fmt.Println()

		turn++
	}
}

func random(size int) int {
	return seededRand.Intn(size)
}

func foundPotion() {
	energy := random(30) + 1
	fmt.Println("You found a potion giving you", energy, "extra strength.")
	strength += energy
}

func foundTreasure() {
	treasure := random(100) + 100
	fmt.Println("Hooray, you found a treasure of", treasure, "coins!")
	coins += treasure
}

func foundTrap() {
	penalty := random(50) + 10
	fmt.Println("Bummer - you sprung a trap. It hurt you", penalty, "strength.")
	strength -= penalty
}

func foundMonster() {
	monsters := [4]string{"snake", "kobold", "goblin", "troll"}
	penalties := [4]int{10, 20, 30, 60}

	monster := random(4)
	bribe := random(75) + 1
	if bribe > coins {
		fmt.Println("A", monsters[monster], "snuck up on you and dealt", bribe, "damage.")
		strength -= bribe;
		return
	}

	monsterType := monsters[monster]
	fmt.Println("Beware - you met a", monsterType)

	fmt.Println("Fight or bribe (f / b)?")
	fmt.Println("(Bribing will cost you", bribe, "coins).")

	for {
		key := getKey()
		if key == 'F' || key == 'f' {
			damage := penalties[monster]
			fmt.Println("You killed the", monsterType, "but it dealt you", damage, "damage")
			strength -= damage
			if monster == 0 {
				if random(100) > 9 {
					return
				}

				fmt.Print("The snake was guarding a treasure - ")
				if random(100) < 3 {
					worth := (random(4) + 1) * 100
					fmt.Println("a huge diamond, worth", worth, "coins!")
					coins += worth
					return
				}

				worth := random(100) + 1
				fmt.Println(worth, "coins ...")
				coins += worth
			}

			return
		}

		if key == 'B' || key == 'b' {
			fmt.Println("... and so you pay", bribe, "coins and run away!")
			coins -= bribe
			return
		}
	}
}

func getKey() rune {
	char, _, err := keyboard.GetSingleKey()
	if err != nil {
		panic(err)
	}

	return char
}
