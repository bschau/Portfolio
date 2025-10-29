package main

import (
	"fmt"
	"math/rand"
	"os"
	"time"
	"unicode"

	"github.com/eiannone/keyboard"
)

var matrix = [8][3]int{
	{8, 7, 6}, {8, 5, 2}, {8, 4, 0}, {7, 4, 1},
	{6, 3, 0}, {6, 4, 2}, {5, 4, 3}, {2, 1, 0},
}
var preferred = [9]int{4, 8, 6, 2, 0, 7, 5, 3, 1}
var board = [9]byte{0, 0, 0, 0, 0, 0, 0, 0, 0}
var seededRand *rand.Rand

func main() {
	s1 := rand.NewSource(time.Now().UnixNano())
	seededRand = rand.New(s1)

	fmt.Println("Tic Tac Toe")
	fmt.Println()
	firstMove := yesNo("Do you want the first move?")
	hardGame := yesNo("Do you want a hard game?")

	initGame(hardGame)
	ticTacToe(firstMove)
	fmt.Println()
}

func yesNo(text string) rune {
	fmt.Print(text, " (y/n) > ")
	response := ' '
	for {
		response = unicode.ToLower(getKey())
		if response == 'y' || response == 'n' {
			break
		}
	}

	fmt.Println(string(response))
	return response
}

func getKey() rune {
	char, _, err := keyboard.GetSingleKey()
	if err != nil {
		panic(err)
	}

	return char
}

func initGame(hardGame rune) {
	if hardGame == 'y' {
		for i := 0; i < 100; i++ {
			// Shuffle corners
			x := seededRand.Intn(4) + 1
			y := seededRand.Intn(4) + 1
			z := preferred[x]
			preferred[x] = preferred[y]
			preferred[y] = z

			// Shuffle the rest
			x = seededRand.Intn(4) + 5
			y = seededRand.Intn(4) + 5
			z = preferred[x]
			preferred[x] = preferred[y]
			preferred[y] = z
		}

		return
	}

	for i := 0; i < 100; i++ {
		// Shuffle all
		x := seededRand.Intn(8) + 1
		y := seededRand.Intn(8) + 1
		z := preferred[x]
		preferred[x] = preferred[y]
		preferred[y] = z
	}

	// Pick middle move
	x := seededRand.Intn(8) + 1
	preferred[0] = preferred[x]
	preferred[x] = 4
}

func ticTacToe(firstMove rune) {
	user := byte('X')
	cpu := byte('O')
	if firstMove == 'n' {
		user, cpu = cpu, user
	}

	if firstMove == 'n' {
		cpuMove := makeMove(cpu, user) + 1
		fmt.Println()
		fmt.Println("Cpu moved to", cpuMove)
	}

	for {
		printBoard()
		findWinner(cpu, user)

		cell := ' '
		fmt.Print("Your move > ")
		for {
			cell = getKey()
			if cell >= '1' && cell <= '9' {
				fmt.Println(string(cell))
				cell -= '1'
				break
			}
		}

		board[cell] = user
		printBoard()
		findWinner(cpu, user)

		cpuMove := makeMove(cpu, user)
		if cpuMove == -1 {
			fmt.Println("Out of moves - game is a draw ...")
			os.Exit(0)
		}

		fmt.Println()
		fmt.Println("User moved to", (cell + 1), "- Cpu moved to", (cpuMove + 1))
	}
}

func printBoard() {
	cells := [9]byte{0, 0, 0, 0, 0, 0, 0, 0, 0}

	for i := 0; i < 9; i++ {
		if board[i] == 0 {
			cells[i] = byte('1' + i)
		} else {
			cells[i] = board[i]
		}
	}

	fmt.Println()
	printRow(cells[0], cells[1], cells[2])
	fmt.Println("---+---+---")
	printRow(cells[3], cells[4], cells[5])
	fmt.Println("---+---+---")
	printRow(cells[6], cells[7], cells[8])
}

func printRow(c1 byte, c2 byte, c3 byte) {
	fmt.Println("   |   |")
	fmt.Printf(" %s | %s | %s", string(c1), string(c2), string(c3))
	fmt.Println()
	fmt.Println("   |   |")
}

func findWinner(cpu byte, user byte) {
	for i := 0; i < 8; i++ {
		x := matrix[i][0]
		y := matrix[i][1]
		z := matrix[i][2]

		// human wins?
		if board[x] == user &&
			board[y] == user &&
			board[z] == user {
			fmt.Println("Congratulations - you win!")
			os.Exit(0)
		}

		// cpu wins?
		if board[x] == cpu &&
			board[y] == cpu &&
			board[z] == cpu {
			fmt.Println("Yay! I win!")
			os.Exit(0)
		}
	}

	// Is it a draw?
	for i := 0; i < 9; i++ {
		if board[i] == 0 {
			return
		}
	}

	fmt.Println("Game is a draw!")
	os.Exit(0)
}

func makeMove(cpu byte, user byte) int {
	// can I win?
	cpuMove := findHole(cpu)
	if cpuMove != -1 {
		board[cpuMove] = cpu
		return cpuMove
	}

	// ok then ... can I block?
	cpuMove = findHole(user)
	if cpuMove != -1 {
		board[cpuMove] = cpu
		return cpuMove
	}

	// no win, no block - select next move from rnd list
	for i := 0; i < 9; i++ {
		cpuMove = preferred[i]
		if board[cpuMove] == 0 {
			board[cpuMove] = cpu
			return cpuMove
		}
	}

	// huh?  No valid moves ...
	return -1
}

func findHole(color byte) int {
	for i := 0; i < 8; i++ {
		x := matrix[i][0]
		y := matrix[i][1]
		z := matrix[i][2]

		if board[x] == color &&
			board[y] == color &&
			board[z] == 0 {
			return z
		}

		if board[x] == color &&
			board[y] == 0 &&
			board[z] == color {
			return y
		}

		if board[x] == 0 &&
			board[y] == color &&
			board[z] == color {
			return x
		}
	}

	return -1
}
