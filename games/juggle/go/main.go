package main

import (
	"fmt"
	"math/rand"
	"os"
	"time"

	"github.com/gdamore/tcell/v2"
)

const FPS = 60

const ScreenW = 26
const ScreenH = 24
const (
	MoveNone = iota
	MoveLeft
	MoveRight
)
const (
	BallO = iota
	BallM
	BallI
)

const PlayerW = 26
const Balls = 3
const BallLength = 28
const BallDead = 0x20
const BallLeftStop = 0x40
const BallRightStop = 0x80
const BallStops = (BallLeftStop | BallRightStop)

var defStyle = tcell.StyleDefault.Foreground(tcell.ColorBlack).Background(tcell.ColorBlack)
var manStyle = tcell.StyleDefault.Foreground(tcell.ColorReset).Background(tcell.ColorBlue)
var eyeStyle = tcell.StyleDefault.Foreground(tcell.ColorReset).Background(tcell.ColorYellow)
var ballStyle = tcell.StyleDefault.Foreground(tcell.ColorReset).Background(tcell.ColorGreen)
var hudStyle = tcell.StyleDefault.Foreground(tcell.ColorWhite).Background(tcell.ColorBlack)
var ballPositions = []int{
	BallDead, BallDead, BallLeftStop | 0, 9, 0, 6, 1, 4, 3, 2, 6, 1, 9, 0, 13, 0, 16, 1, 19, 2, 21, 4, 22, 6, BallRightStop | 22, 9, BallDead, BallDead,
	BallDead, BallDead, BallLeftStop | 2, 9, 2, 7, 3, 5, 5, 3, 9, 2, 13, 2, 17, 3, 19, 5, 20, 7, BallRightStop | 20, 9, BallDead, BallDead, -1, -1, -1, -1,
	BallDead, BallDead, BallLeftStop | 4, 9, 5, 7, 7, 6, 10, 4, 12, 4, 15, 6, 17, 7, BallRightStop | 18, 9, BallDead, BallDead, -1, -1, -1, -1, -1, -1, -1, -1,
}

var ballIndices = []int{12, 12, 8}
var ballSpeeds = []int{-2, 2, -2}
var ballAnim = 1
var ballDying = []bool{false, false, false}
var ballSaved = []bool{false, false, false}
var seededRand *rand.Rand
var score = 0
var move = MoveNone
var playerAnim = MoveNone
var dying = false
var dyingAnim = 0

func main() {
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

	screen.HideCursor()
	screen.SetStyle(defStyle)
	screen.Clear()

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

	fmt.Println("Final score:", score)

	if maybePanic != nil {
		panic(maybePanic)
	}
}

func drawText(screen tcell.Screen, x int, y int, style tcell.Style, text string) {
	for _, r := range []rune(text) {
		screen.SetContent(x, y, r, nil, style)
		x++
	}
}

func update(screen tcell.Screen) {
	for i := 0; i < Balls; i++ {
		x := (getBallStopX(i) & ^BallStops) + 1
		y := (getBallStopY(i) & ^BallStops) + 3
		screen.SetContent(x, y, ' ', nil, defStyle)
	}

	if !dying {
		handleInGameMode()
	}
	player := getPlayerAnim(playerAnim)

	y := 11
	x := 0
	for i := 0; i < len(player); i++ {
		if player[i] == ' ' {
			screen.SetContent(x, y, ' ', nil, defStyle)
		} else if player[i] == '*' {
			screen.SetContent(x, y, ' ', nil, manStyle)
		} else if player[i] == '#' {
			screen.SetContent(x, y, ' ', nil, eyeStyle)
		}

		x++
		if x == PlayerW-1 {
			x = 0
			y++
		}
	}

	if mustShowBalls() {
		for i := 0; i < Balls; i++ {
			x := (getBallStopX(i) & ^BallStops) + 1
			y := (getBallStopY(i) & ^BallStops) + 3
			screen.SetContent(x, y, ' ', nil, ballStyle)
		}
	}

	sc := fmt.Sprintf("Score: %d", score)
	drawText(screen, 0, ScreenH-1, hudStyle, sc)

	screen.Show()

	if dying {
		dyingAnim++
		if dyingAnim > 200 {
			gameExit(screen)
			os.Exit(0)
		}
	}
}

func getPlayerAnim(anim int) string {
	if playerAnim == MoveLeft {
		return PlayerL
	} else if playerAnim == MoveRight {
		return PlayerR
	}

	return PlayerM
}

func mustShowBalls() bool {
	if !dying {
		return true
	}

	return (dyingAnim/20)%2 == 0
}

func handleInGameMode() {
	if move == MoveLeft {
		if playerAnim == MoveRight {
			playerAnim = MoveNone
		} else if playerAnim == MoveNone {
			playerAnim = MoveLeft
		}
	} else if move == MoveRight {
		if playerAnim == MoveNone {
			playerAnim = MoveRight
		} else if playerAnim == MoveLeft {
			playerAnim = MoveNone
		}
	}
	move = MoveNone
	saveBalls()
	moveBalls()
	dying = isDying()
}

func moveBalls() {
	ballAnim--
	if ballAnim > 0 {
		saveBalls()
		return
	}

	ballAnim = getBallAnim()

	for i := 0; i < Balls; i++ {
		if ballSaved[i] {
			ballSpeeds[i] = -ballSpeeds[i]
			ballSaved[i] = false
			score++
		}

		previous := ballIndices[i]
		ballIndices[i] += ballSpeeds[i]

		p := getBallStopX(i)
		if p == BallDead {
			ballDying[i] = true
			ballIndices[i] = previous
		}
	}
}

func getBallAnim() int {
	const MaxThrottle = 900
	if score > MaxThrottle {
		return 1
	}

	const Base = 31
	s := score / Base
	return Base - s
}

func saveBalls() {
	if playerAnim == MoveLeft {
		p := getBallStopX(BallO)
		if (p & BallLeftStop) != 0 {
			ballSaved[BallO] = true
		}

		p = getBallStopX(BallI)
		if (p & BallRightStop) != 0 {
			ballSaved[BallI] = true
		}
	} else if playerAnim == MoveNone {
		p := getBallStopX(BallM)
		if (p & BallStops) != 0 {
			ballSaved[BallM] = true
		}
	} else if playerAnim == MoveRight {
		p := getBallStopX(BallO)
		if (p & BallRightStop) != 0 {
			ballSaved[BallO] = true
		}

		p = getBallStopX(BallI)
		if (p & BallLeftStop) != 0 {
			ballSaved[BallI] = true
		}
	}
}

func getBallStopX(ball int) int {
	return ballPositions[(ball*BallLength)+ballIndices[ball]]
}

func getBallStopY(ball int) int {
	return ballPositions[(ball*BallLength)+ballIndices[ball]+1]
}

func isDying() bool {
	return ballDying[BallO] || ballDying[BallM] || ballDying[BallI]
}
