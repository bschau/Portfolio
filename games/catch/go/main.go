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
const BucketY = 19
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

var defStyle = tcell.StyleDefault.Foreground(tcell.ColorBlack).Background(tcell.ColorBlack)
var hudStyle = tcell.StyleDefault.Foreground(tcell.ColorWhite).Background(tcell.ColorBlack)
var platformStyle = tcell.StyleDefault.Foreground(tcell.ColorPurple).Background(tcell.ColorBlack)
var chequeredStyle = tcell.StyleDefault.Foreground(tcell.ColorBlue).Background(tcell.ColorWhite)
var ballStyle = tcell.StyleDefault.Foreground(tcell.ColorYellow).Background(tcell.ColorBlack)
var seededRand *rand.Rand
var score int
var ballX int
var ballY int
var bucketX int
var shieldX int
var shieldY int
var levelDelay int
var delay int
var move int
var gameMode int
var flashCount int

func main() {
	levelDelay = 16
	score = 0

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

	newLevel(screen)

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
	fmt.Println("Your final score:", score)
	if maybePanic != nil {
		panic(maybePanic)
	}
}

func newLevel(screen tcell.Screen) {
	screen.HideCursor()
	screen.SetStyle(defStyle)
	screen.Clear()

	for row := 0; row < ScreenH; row++ {
		screen.SetContent(0, row, tcell.RuneVLine, nil, platformStyle)
		screen.SetContent(ScreenW-1, row, tcell.RuneVLine, nil, platformStyle)
	}

	for col := 0; col < ScreenW; col++ {
		screen.SetContent(col, ScreenH-1, tcell.RuneHLine, nil, platformStyle)
	}

	screen.SetContent(0, ScreenH-1, tcell.RuneLLCorner, nil, platformStyle)
	screen.SetContent(ScreenW-1, ScreenH-1, tcell.RuneLRCorner, nil, platformStyle)

	bucketX = seededRand.Intn(ScreenW-5) + 1
	screen.SetContent(bucketX, BucketY, tcell.RuneLLCorner, nil, platformStyle)
	screen.SetContent(bucketX+1, BucketY, tcell.RuneCkBoard, nil, chequeredStyle)
	screen.SetContent(bucketX+2, BucketY, tcell.RuneLRCorner, nil, platformStyle)

	ballY = 1
	ballX = seededRand.Intn(ScreenW-2) + 1
	delay = levelDelay
	move = MoveNone

	shieldX = seededRand.Intn(4) - 3 + bucketX - 1
	if shieldX < 3 {
		shieldX = 3
	} else if shieldX > 29 {
		shieldX = 29
	}

	shieldY = seededRand.Intn(8) + 10
	screen.SetContent(shieldX, shieldY, tcell.RuneHLine, nil, platformStyle)
	screen.SetContent(shieldX+1, shieldY, tcell.RuneHLine, nil, platformStyle)
	screen.SetContent(shieldX+2, shieldY, tcell.RuneHLine, nil, platformStyle)
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
	drawText(screen, (ScreenW-len(sc))/2, 0, hudStyle, sc)

	if gameMode == InGame {
		handleInGameMode(screen)
	} else if gameMode == Blinking || gameMode == Dead {
		handleBlinkingMode(screen)
	}

	screen.Show()
}

func handleInGameMode(screen tcell.Screen) {
	var x = ballX
	var y = ballY

	delay--
	if delay < 0 {
		delay = levelDelay
		ballY++
	}

	if move == MoveLeft {
		if ballX > 1 {
			ballX--
		}
	} else if move == MoveRight {
		if ballX < ScreenW-2 {
			ballX++
		}
	}

	move = MoveNone

	if x != ballX || y != ballY {
		screen.SetContent(x, y, ' ', nil, ballStyle)
		screen.SetContent(ballX, ballY, '֍', nil, ballStyle)
	}

	if isCaught() {
		score++
		delay = 16
		flashCount = 0
		gameMode = Blinking
	} else if collision() {
		delay = 16
		flashCount = 0
		gameMode = Dead
	}
}

func isCaught() bool {
	return ballX == bucketX+1 && ballY == BucketY
}

func collision() bool {
	return (ballY == ScreenH-1) ||
		(ballY == BucketY && (ballX == bucketX || ballX == bucketX+2)) ||
		(ballY == shieldY && ballX >= shieldX && ballX <= shieldX+2)
}

func handleBlinkingMode(screen tcell.Screen) {
	delay--
	if delay > 0 {
		return
	}
	delay = 16

	flashCount++
	if flashCount%2 == 0 {
		screen.SetContent(ballX, ballY, ' ', nil, ballStyle)
	} else {
		screen.SetContent(ballX, ballY, '֍', nil, ballStyle)
	}

	if flashCount > 6 {
		if gameMode == Dead {
			gameExit(screen)
			os.Exit(0)
		}

		levelDelay--
		if levelDelay < 0 {
			levelDelay = 0
		}
		newLevel(screen)
	}
}
