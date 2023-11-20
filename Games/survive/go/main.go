package main

import (
	"fmt"
	"math/rand"
	"os"
	"time"

	"github.com/gdamore/tcell/v2"
)

const FPS = 60
const ScreenW = 32
const ScreenH = 22
const (
	MoveNone = iota
	MoveLeft
	MoveRight
	MoveUp
	MoveDown
)

const (
	Space = iota
	Player
	Coin
	Home
	Hole
	Bomb
)

const (
	Playing = iota
	Exited
	Bombed
)

const BombAnimatingReset int = 6

var defStyle = tcell.StyleDefault.Foreground(tcell.ColorBlack).Background(tcell.ColorBlack)
var hudStyle = tcell.StyleDefault.Foreground(tcell.ColorWhite).Background(tcell.ColorBlack)
var spaceStyle = tcell.StyleDefault.Foreground(tcell.ColorReset).Background(tcell.ColorBlack)
var coinStyle = tcell.StyleDefault.Foreground(tcell.ColorYellow).Background(tcell.ColorBlack)
var playerStyle = tcell.StyleDefault.Foreground(tcell.ColorBlue).Background(tcell.ColorBlack)
var homeStyle = tcell.StyleDefault.Foreground(tcell.ColorGreen).Background(tcell.ColorBlack)
var bombStyle = tcell.StyleDefault.Foreground(tcell.ColorOrange).Background(tcell.ColorBlack)
var bombAnimation = []int{Bomb, Space, Bomb, Space, Bomb}
var score int = 0
var seededRand *rand.Rand
var playfield []int
var playerX int
var playerY int
var move int
var gameMode int
var bombX int
var bombY int
var bombAnimating int
var bombFrame int

func main() {
	playfield = make([]int, ScreenH*ScreenW)
	if playfield == nil {
		panic("Out of memory")
	}

	s1 := rand.NewSource(time.Now().UnixNano())
	seededRand = rand.New(s1)

	screen, err := tcell.NewScreen()
	if err != nil {
		panic(err)
	}

	if err := screen.Init(); err != nil {
		panic(err)
	}

	width, height := screen.Size()
	if width < ScreenW || height < ScreenH {
		panic(fmt.Sprintf("Please resize screen to at least %d x %d characters", ScreenW, ScreenH))
	}

	ticker := time.NewTicker(time.Second / FPS)
	done := make(chan bool)

	quit := func() {
		done <- true
		gameExit(screen)
	}
	defer quit()

	initGame(screen)

	go func(screen tcell.Screen) {
		for {
			select {
			case <-done:
				return
			case <-ticker.C:
				update(screen)
			}
		}
	}(screen)

	for {
		ev := screen.PollEvent()
		switch ev := ev.(type) {
		case *tcell.EventKey:
			var key = ev.Key()
			if key == tcell.KeyEscape || key == tcell.KeyCtrlC {
				return
			}

			if key == tcell.KeyLeft {
				move = MoveLeft
			} else if key == tcell.KeyRight {
				move = MoveRight
			} else if key == tcell.KeyUp {
				move = MoveUp
			} else if key == tcell.KeyDown {
				move = MoveDown
			} else {
				move = MoveNone
			}
		}
	}
}

func gameExit(screen tcell.Screen) {
	maybePanic := recover()
	screen.Clear()
	screen.ShowCursor(0, 0)
	screen.Fini()

	if gameMode == Exited {
		fmt.Println("You made it safe home. Final score:", score)
	} else if gameMode == Bombed {
		fmt.Println("You suffered a horrible death! Final score:", score)
	} else {
		fmt.Println("Goodbye. Score so far:", score)
	}

	if maybePanic != nil {
		panic(maybePanic)
	}
}

func initGame(screen tcell.Screen) {
	playerX = 0
	playerY = (ScreenH - 1) / 2

	screen.SetStyle(defStyle)
	screen.HideCursor()
	screen.Clear()

	for i := 0; i < ScreenW*(ScreenH-1); i++ {
		playfield[i] = Coin
	}

	homeY := seededRand.Intn(ScreenH - 1)
	playfield[(homeY*ScreenW)+ScreenW-1] = Home
	bombX = -1
	bombY = -1
	bombAnimating = 100
	bombFrame = 0
	gameMode = Playing
}

func drawText(screen tcell.Screen, x int, y int, style tcell.Style, text string) {
	for _, r := range []rune(text) {
		screen.SetContent(x, y, r, nil, style)
		x++
	}
}

func update(screen tcell.Screen) {
	sc := fmt.Sprintf("Score: %d  ", score)
	drawText(screen, 0, ScreenH-1, hudStyle, sc)

	animateBomb(screen)
	movePlayer(screen)
	c := -1
	for row := 0; row < ScreenH-1; row++ {
		for col := 0; col < ScreenW; col++ {
			c++
			if col == playerX && row == playerY {
				screen.SetContent(col, row, ' ', nil, defStyle)
				continue
			}

			if playfield[c] == Space {
				screen.SetContent(col, row, ' ', nil, defStyle)
			} else if playfield[c] == Coin {
				screen.SetContent(col, row, '.', nil, coinStyle)
			} else if playfield[c] == Home {
				screen.SetContent(col, row, '#', nil, bombStyle)
			} else if playfield[c] == Hole || playfield[c] == Bomb {
				screen.SetContent(col, row, '*', nil, bombStyle)
			}
		}
	}

	screen.SetContent(playerX, playerY, '@', nil, playerStyle)
	screen.Sync()
	playerHome(screen)
}

func movePlayer(screen tcell.Screen) {
	x := playerX
	y := playerY
	if move == MoveLeft {
		if playerX > 0 {
			playerX--
		}
	} else if move == MoveRight {
		if playerX < ScreenW-1 {
			playerX++
		}
	} else if move == MoveUp {
		if playerY > 0 {
			playerY--
		}
	} else if move == MoveDown {
		if playerY < ScreenH-2 {
			playerY++
		}
	}

	move = MoveNone

	if x == playerX && y == playerY {
		return
	}

	c := (playerY * ScreenW) + playerX
	if playfield[c] == Coin {
		score += 2
		playfield[c] = Space
	} else if playfield[c] == Hole {
		score -= 3
	}
}

func playerHome(screen tcell.Screen) {
	if playfield[(playerY*ScreenW)+playerX] != Home {
		return
	}

	gameMode = Exited
	gameExit(screen)
	os.Exit(0)
}

func animateBomb(screen tcell.Screen) {
	if bombX == -1 {
		startBombing()
		return
	}

	idx := (bombY * ScreenW) + bombX
	playfield[idx] = bombAnimation[bombFrame]

	if bombAnimating > 0 {
		bombAnimating--
		return
	}

	bombAnimating = BombAnimatingReset
	if bombFrame < len(bombAnimation)-1 {
		bombFrame++
		return
	}

	playfield[(bombY*ScreenW)+bombX] = Hole
	if playerX == bombX && playerY == bombY {
		gameMode = Bombed
		gameExit(screen)
		os.Exit(0)
	}

	bombX = -1
	bombY = -1
}

func startBombing() {
	bombFrame = 0
	bombAnimating = BombAnimatingReset

	for {
		bombX = playerX + seededRand.Intn(3) - 1
		if bombX < 0 {
			bombX = 0
		} else if bombX > ScreenW-1 {
			bombX = ScreenW - 1
		}
		bombY = playerY + seededRand.Intn(3) - 1
		if bombY < 0 {
			bombY = 0
		} else if bombY > ScreenH-2 {
			bombY = ScreenH - 2
		}

		if playfield[(bombY*ScreenW)+bombX] != Home {
			return
		}
	}
}
