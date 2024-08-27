package main

import (
	"fmt"
	"math/rand"
	"time"
)

type Army struct {
	airplanes           int
	ships               int
	soldiers            int
	airplanesProduction int
	shipsProduction     int
}

var seededRand *rand.Rand
var east Army
var west Army

func main() {
	s1 := rand.NewSource(time.Now().UnixNano())
	seededRand = rand.New(s1)

	fmt.Println("Wargame")
	fmt.Println("=======")
	fmt.Println()
	fmt.Println("West vs East ... you play west, I play east.")
	fmt.Println("Prepare to die!")
	fmt.Println()
	wargame()
	fmt.Println()
}

func wargame() {
	east = newArmy()
	west = newArmy()

	day := 1
	for {
		fmt.Println()
		fmt.Println("Conflict, day", day)
		showArmy(west, "You")
		showArmy(east, "Me")

		fmt.Println()
		fmt.Println("Awaiting your orders, General!")
		fmt.Printf("  %s Bomb airfields  %s Bomb harbour            %s Wage war at sea", getCommand(west.airplanes, "1."), getCommand(west.airplanes, "2."), getCommand(west.ships, "3."))
		fmt.Println()
		fmt.Printf("  %s Invade East     %s Bomb aircraft factory   %s Bomb harbour dock", getCommand(west.soldiers, "4."), getCommand(west.airplanes, "5."), getCommand(west.airplanes, "6."))
		fmt.Println()
		fmt.Printf("  7. Do nothing")
		fmt.Println()
		fmt.Println()
		order := getOrder()

		fmt.Println()
		fmt.Println()
		fmt.Println("===============================================================================")
		fmt.Println()
		if !handleOrder(&west, &east, order, "You", "east") {
			fmt.Println()
			fmt.Println("Cannot do that ...")
			fmt.Println()
			continue
		}

		handleResponse(&west, &east)
		adjustProduction()

		day++
		if day > 25 {
			break
		}
	}

	total_west := west.airplanes + west.ships + west.soldiers
	total_east := east.airplanes + east.ships + east.soldiers

	fmt.Println()
	fmt.Println()
	fmt.Println("The war is over!")
	fmt.Println()
	fmt.Println()
	if total_west < total_east {
		fmt.Println("E A S T wins this battle .. Sorry General, you're demoted to soldier!")
	} else if total_west > total_east {
		fmt.Println("Way to go, General! You won!")
		fmt.Println("And in the later years you retire, write a book about it and become")
		fmt.Println("filthy rich. But - hey! You earned it! :-)")
	} else {
		fmt.Println("Peace!")
	}
}

func newArmy() Army {
	return Army{
		airplanes:           100,
		ships:               100,
		soldiers:            1000,
		airplanesProduction: 9,
		shipsProduction:     9,
	}
}

func showArmy(army Army, label string) {
	fmt.Println(label + ":")
	fmt.Printf("  Airplanes: %d (%d), ships: %d (%d), soldiers: %d",
		army.airplanes, army.airplanesProduction,
		army.ships, army.shipsProduction, army.soldiers)
	fmt.Println()
}

func getCommand(amount int, command string) string {
	if amount > 0 {
		return command
	}

	return "  "
}

func getOrder() int {
	for {
		fmt.Printf("> ")
		var order int
		n, _ := fmt.Scanf("%d", &order)
		if n != 1 {
			continue
		}

		if order >= 1 && order <= 7 {
			return order
		}
	}
}

func handleOrder(attacker *Army, defender *Army, order int, attackerName string, defenderName string) bool {
	var ret bool

	switch order {
	case 1:
		attacker.airplanes, defender.airplanes, ret = battle(attacker.airplanes, defender.airplanes, "%s bombed %s airfields, dealing %d damage while losing %d airplane", attackerName, defenderName)
		return ret

	case 2:
		attacker.airplanes, defender.ships, ret = battle(attacker.airplanes, defender.ships, "%s boombed %s harbour, dealing %d damage while losing %d airplane", attackerName, defenderName)
		return ret

	case 3:
		attacker.ships, defender.ships, ret = battle(attacker.ships, defender.ships, "%s battled %s at sea, dealing %d damage while losing %d ship", attackerName, defenderName)
		return ret

	case 4:
		attacker.soldiers, defender.soldiers, ret = battle(attacker.soldiers, defender.soldiers, "%s invaded %s, dealing %d damage while losing %d soldier", attackerName, defenderName)
		return ret

	case 5:
		attacker.airplanes, defender.airplanesProduction, ret = battle(attacker.airplanes, defender.airplanesProduction, "%s bombed %s aircraft factories, dealing %d damage while losing %d airplane", attackerName, defenderName)
		return ret

	case 6:
		attacker.airplanes, defender.shipsProduction, ret = battle(attacker.airplanes, defender.shipsProduction, "%s bombed %s harbour docks, dealing %d damage while losing %d airplane", attackerName, defenderName)
		return ret

	case 7:
		fmt.Println("Nothing was done ...")
		fmt.Println()
		return true
	}

	return false
}

func battle(attacker int, defender int, text string, attackerName string, defenderName string) (int, int, bool) {
	if attacker < 1 {
		return attacker, defender, false
	}

	if defender < 1 {
		return attacker, defender, true
	}

	attDamage := seededRand.Intn(defender) / 9
	attacker -= attDamage
	if attacker < 0 {
		attacker = 0
	}

	defDamage := seededRand.Intn(attacker) / 10
	defender -= defDamage
	if defender < 0 {
		defender = 0
	}

	if attDamage != 1 {
		text += "s."
	}

	fmt.Printf(text, attackerName, defenderName, defDamage, attDamage)
	fmt.Println()
	fmt.Println()
	return attacker, defender, true
}

func handleResponse(west *Army, east *Army) {
	choices := []int{}

	if east.airplanes > 0 {
		if west.airplanes > 0 {
			choices = append(choices, 1)
		}

		if west.ships > 0 {
			choices = append(choices, 2)
		}

		if west.airplanesProduction > 0 {
			choices = append(choices, 5)
		}

		if west.shipsProduction > 0 {
			choices = append(choices, 6)
		}
	}

	if east.ships > 0 && west.ships > 0 {
		choices = append(choices, 3)
	}

	if east.soldiers > 0 && west.soldiers > 0 {
		choices = append(choices, 4)
	}

	if len(choices) == 0 {
		fmt.Println("East is idle.")
		return
	}

	choice := choices[seededRand.Intn(len(choices))]
	handleOrder(east, west, choice, "East", "your")
}

func adjustProduction() {
	west.airplanes += getProduction(west.airplanesProduction, "airplane", "You")
	west.ships += getProduction(west.shipsProduction, "ship", "You")
	east.airplanes += getProduction(east.airplanesProduction, "airplane", "I")
	east.ships += getProduction(east.shipsProduction, "ship", "I")
}

func getProduction(production int, material string, who string) int {
	if seededRand.Intn(50) < 20 {
		production /= 2
	}

	if production != 1 {
		material += "s"
	}

	fmt.Println(who, "build", production, material)
	return production
}
