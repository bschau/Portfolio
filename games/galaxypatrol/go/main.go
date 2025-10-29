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
const PlayerY = 3
const (
	MoveNone = iota
	MoveLeft
	MoveRight
)
const (
	Dead = iota
	InGame
	Blinking
)

const (
	Space = iota
	Star
	Fuel
)

var defStyle = tcell.StyleDefault.Foreground(tcell.ColorBlack).Background(tcell.ColorBlack)
var spaceStyle = tcell.StyleDefault.Foreground(tcell.ColorReset).Background(tcell.ColorLightGray)
var hudStyle = tcell.StyleDefault.Foreground(tcell.ColorBlack).Background(tcell.ColorLightGray)
var playerStyle = tcell.StyleDefault.Foreground(tcell.ColorBlue).Background(tcell.ColorLightGray)
var fuelStyle = tcell.StyleDefault.Foreground(tcell.ColorGreen).Background(tcell.ColorLightGray)
var starStyle = tcell.StyleDefault.Foreground(tcell.ColorRed).Background(tcell.ColorLightGray)
var score int = 0
var fuel int = 150
var numStars int = 1
var seededRand *rand.Rand
var playfield []int
var playerX int
var levelDelay int = 16
var delay int = levelDelay
var move int
var gameMode int
var flashCount int

func main() {
	playfield = make([]int, (ScreenH-1)*ScreenW)
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
		if gameMode != InGame {
			continue
		}

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
	if fuel < 1 {
		fmt.Println("You ran out of fuel. Final score:", score)
	} else {
		fmt.Println("Crash! Final score:", score)
	}

	if maybePanic != nil {
		panic(maybePanic)
	}
}

func initGame(screen tcell.Screen) {
	playerX = ScreenW / 2

	screen.SetStyle(defStyle)
	screen.HideCursor()
	screen.Clear()

	for row := 0; row < ScreenH; row++ {
		for col := 0; col < ScreenW; col++ {
			screen.SetContent(col, row, ' ', nil, spaceStyle)
		}
	}

	screen.SetContent(playerX, PlayerY, 'V', nil, playerStyle)
	gameMode = InGame
}

func drawText(screen tcell.Screen, x int, y int, style tcell.Style, text string) {
	for _, r := range []rune(text) {
		screen.SetContent(x, y, r, nil, style)
		x++
	}
}

func update(screen tcell.Screen) {
	sc := fmt.Sprintf("Score: %d", score)
	drawText(screen, 0, 0, hudStyle, sc)
	sc = fmt.Sprintf("      Fuel: %d", fuel)
	drawText(screen, ScreenW-len(sc), 0, hudStyle, sc)

	if gameMode == InGame {
		handleInGameMode(screen)
	} else if gameMode == Blinking || gameMode == Dead {
		handleBlinkingMode(screen)
	}

	screen.Show()
}

func handleInGameMode(screen tcell.Screen) {
	if move == MoveLeft {
		if playerX > 0 {
			playerX--
		}
	} else if move == MoveRight {
		if playerX < ScreenW-1 {
			playerX++
		}
	}

	move = MoveNone

	delay--
	if delay < 0 {
		delay = levelDelay
		moveStars()
		score++
		fuel--
	}

	index := 0
	for y := 1; y < ScreenH; y++ {
		for x := 0; x < ScreenW; x++ {
			if playfield[index] == Space {
				screen.SetContent(x, y, ' ', nil, spaceStyle)
			} else if playfield[index] == Fuel {
				screen.SetContent(x, y, '#', nil, fuelStyle)
			} else {
				screen.SetContent(x, y, '*', nil, starStyle)
			}
			index++
		}
	}

	screen.SetContent(playerX, PlayerY, 'V', nil, playerStyle)

	if collision() {
		delay = 16
		flashCount = 0
		gameMode = Dead
	} else if pickup() {
		fuel += 5
	} else if fuel < 1 {
		delay = 16
		flashCount = 0
		gameMode = Dead
	}
}

func moveStars() {
	dst := 0
	src := ScreenW
	for index := 0; index < ((ScreenH - 2) * ScreenW); index++ {
		playfield[dst] = playfield[src]
		dst++
		src++
	}

	dst = (ScreenH - 2) * ScreenW
	for index := 0; index < ScreenW; index++ {
		playfield[dst] = 0
		dst++
	}

	dst = (ScreenH - 2) * ScreenW
	for index := 0; index < numStars; index++ {
		x := seededRand.Intn(ScreenW)
		playfield[dst+x] = Star
	}

	if seededRand.Intn(100) < 25 {
		playfield[dst+seededRand.Intn(ScreenW)] = Fuel
	}

	step := (score / 200) + 1
	if step > 7 {
		step = 10
	}
	numStars = step

	if score%50 == 0 {
		if levelDelay > 1 {
			levelDelay--
		}
	}
}

func collision() bool {
	index := ((PlayerY - 1) * ScreenW) + playerX
	return playfield[index] == Star
}

func pickup() bool {
	index := ((PlayerY - 1) * ScreenW) + playerX
	if playfield[index] != Fuel {
		return false
	}

	playfield[index] = Space
	return true
}

func handleBlinkingMode(screen tcell.Screen) {
	delay--
	if delay > 0 {
		return
	}
	delay = 16

	flashCount++
	if flashCount%2 == 0 {
		if fuel < 1 {
			screen.SetContent(playerX, PlayerY, ' ', nil, spaceStyle)
		} else {
			screen.SetContent(playerX, PlayerY, '*', nil, starStyle)
		}
	} else {
		screen.SetContent(playerX, PlayerY, 'V', nil, playerStyle)
	}

	if flashCount > 6 {
		if gameMode == Dead {
			gameExit(screen)
			os.Exit(0)
		}
	}
}
