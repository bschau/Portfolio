package main

import ( "fmt"; "math/rand"; "os"; "time" )

var preferred = [9]int { 4, 8, 6, 2, 0, 7, 5, 3, 1 }
var matrix = [8][3]int {
	{ 8, 7, 6 }, { 8, 5, 2 }, { 8, 4, 0 }, { 7, 4, 1},
	{ 6, 3, 0 }, { 6, 4, 2 }, { 5, 4, 3 }, { 2, 1, 0},
}
var board = [9]byte { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
var user byte
var cpu byte
var hard bool
var cpuMove int

func main() {
	parseArguments()

	initGame()
	showGameType()

	if cpu == 'X' {
		makeMove()
		fmt.Println()
		fmt.Printf("Cpu moved to %d", cpuMove + 1)
		fmt.Println()
	}

	gameLoop()
}

func parseArguments() {
	user = byte('X')
	cpu = byte('O')
	hard = false
	if len(os.Args) == 1 {
		return
	}

	if len(os.Args) != 3 {
		fmt.Println("ttt [x|o e|h]")
		os.Exit(1)
	}

	if os.Args[1][0] == 'o' {
		user = byte('O')
		cpu = byte('X')
	} else if os.Args[1][0] != 'x' {
		fmt.Println("You must specify x or o for user")
		os.Exit(1)
	}

	if os.Args[2][0] == 'h' {
		hard = true
	} else if os.Args[2][0] != 'e' {
		fmt.Println("You must specify e or h for level")
		os.Exit(1)
	}
}

func showGameType() {
	fmt.Print("Level is ")
	if hard {
		fmt.Print("hard")
	} else {
		fmt.Print("easy")
	}

	fmt.Printf(", User is %s and Cpu is %s", string(user), string(cpu))
	fmt.Println()
}

func initGame() {
	rand.Seed(time.Now().UnixNano())

	if hard {
		for i := 0; i < 100; i++ {
			// Shuffle corners
			x := (rand.Int() & 3) + 1;
			y := (rand.Int() & 3) + 1;
			z := preferred[x];
			preferred[x] = preferred[y];
			preferred[y] = z;

			// Shuffle the rest
			x = (rand.Int() & 3) + 5;
			y = (rand.Int() & 3) + 5;
			z = preferred[x];
			preferred[x] = preferred[y];
			preferred[y] = z;
		}
		return
	}

	for i := 0; i < 100; i++ {
		// Shuffle all
		x := (rand.Int() & 7) + 1;
		y := (rand.Int() & 7) + 1;
		z := preferred[x];
		preferred[x] = preferred[y];
		preferred[y] = z;
	}

	// Pick middle move
	x := (rand.Int() & 7) + 1;
	preferred[0] = preferred[x];
	preferred[x] = 4;
}

func gameLoop() {
	for {
		printBoard()
		findWinner()

		cell := prompt()
		board[cell] = user

		makeMove()

		fmt.Println()
		fmt.Printf("User moved to %d, Cpu moved to %d", cell + 1, cpuMove + 1)
		fmt.Println()
	}
}

func printBoard() {
	cells := [9]byte { 0, 0, 0, 0, 0, 0, 0, 0, 0 }

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

func findWinner() {
	for i := 0; i < 8; i++ {
		x := matrix[i][0];
		y := matrix[i][1];
		z := matrix[i][2];

		/* human wins? */
		if board[x] == user &&
		   board[y] == user &&
		   board[z] == user {
			fmt.Println("Congratulations - you win!")
			os.Exit(0)
		}

		/* cpu wins? */
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
			return;
		}
	}

	fmt.Println("Game is a draw!")
	os.Exit(0)
}

func prompt() int {
	for {
		fmt.Print("Enter move > ")

		var cell = 0
		items, _ := fmt.Scanf("%d", &cell)
		if items != 1 {
			continue
		}

		if cell < 1 || cell > 9 || board[cell - 1] != 0 {
			continue
		}

		return cell - 1
	}
}

func makeMove() {
	/* can I win? */
	cpuMove = findHole(cpu)
	if cpuMove != -1 {
		board[cpuMove] = cpu;
		return;
	}

	/* ok then ... can I block? */
	cpuMove = findHole(user)
	if cpuMove != -1 {
		board[cpuMove] = cpu;
		return;
	}

	/* no win, no block - select next move from rnd list */
	for i := 0; i < 9; i++ {
		cpuMove = preferred[i];
		if board[cpuMove] == 0 {
			board[cpuMove] = cpu;
			return;
		}
	}

	/* huh?  No valid moves ... */
	fmt.Println("Out of moves - game is a draw ...")
	os.Exit(0)
}

func findHole(color byte) int {
	for i := 0; i < 8; i++ {
		x := matrix[i][0];
		y := matrix[i][1];
		z := matrix[i][2];

		if board[x] == color &&
		   board[y] == color &&
		   board[z] == 0 {
			return z;
		}

		if board[x] == color &&
		   board[y] == 0 &&
		   board[z] == color {
			return y;
		}

		if board[x] == 0 &&
		   board[y] == color &&
		   board[z] == color {
			return x;
		}
         }

         return -1
}
