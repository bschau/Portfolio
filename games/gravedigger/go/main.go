package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/gdamore/tcell/v2"
)

const ScreenW = 20
const ScreenH = 10
const (
	Space = iota
	Wall
	Grave
	Hole
)
const Zombies = 3
const MaxGraves = 20

var defStyle = tcell.StyleDefault.Foreground(tcell.ColorBlack).Background(tcell.ColorBlack)
var hudStyle = tcell.StyleDefault.Foreground(tcell.ColorWhite).Background(tcell.ColorBlack)
var spaceStyle = tcell.StyleDefault.Foreground(tcell.ColorReset).Background(tcell.ColorBlack)
var wallStyle = tcell.StyleDefault.Foreground(tcell.ColorBlue).Background(tcell.ColorBlack)
var graveStyle = tcell.StyleDefault.Foreground(tcell.ColorGreen).Background(tcell.ColorBlack)
var playerStyle = tcell.StyleDefault.Foreground(tcell.ColorYellow).Background(tcell.ColorBlack)
var holeStyle = tcell.StyleDefault.Foreground(tcell.ColorPurple).Background(tcell.ColorBlack)
var zombieStyle = tcell.StyleDefault.Foreground(tcell.ColorGreen).Background(tcell.ColorBlack)
var seededRand *rand.Rand
var playfield []int
var zombiesX []int
var zombiesY []int
var playerX int
var playerY int
var holesLeft int
var holePlaced bool
var timeLeft int
var fellInHole bool
var playerCaught bool
var exited bool

func main() {
	playfield = make([]int, (ScreenH * ScreenW))
	zombiesX = make([]int, Zombies)
	zombiesY = make([]int, Zombies)
	if playfield == nil || zombiesX == nil || zombiesY == nil {
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

	quit := func() {
		gameExit(screen)
	}
	defer quit()

	width, height := screen.Size()
	if width < ScreenW || height < ScreenH {
		panic(fmt.Sprintf("Please resize screen to at least %d x %d characters", ScreenW, ScreenH))
	}

	initGame(screen)
	for {
		update(screen)
		ev := screen.PollEvent()
		switch ev := ev.(type) {
		case *tcell.EventKey:
			var key = ev.Key()
			if key == tcell.KeyEscape || key == tcell.KeyCtrlC {
				return
			}

			holePlaced = false
			moved := false
			if key == tcell.KeyLeft {
				movePlayer(-1, 0)
				moved = true
			} else if key == tcell.KeyRight {
				movePlayer(1, 0)
				moved = true
			} else if key == tcell.KeyUp {
				movePlayer(0, -1)
				moved = true
			} else if key == tcell.KeyDown {
				movePlayer(0, 1)
				moved = true
			} else if key == tcell.KeyRune {
				if ev.Rune() == ' ' {
					placeHole()
				}
			}

			if moved || holePlaced {
				if playerExited() {
					return
				}

				if !holePlaced && playerInHole() {
					return
				}
				moveZombies(key)
				zombieInHole()

				if zombieCaughtPlayer() {
					return
				}

				timeLeft--
				if timeLeft < 1 {
					update(screen)
					return
				}
			}
		}
	}
}

func gameExit(screen tcell.Screen) {
	maybePanic := recover()
	screen.Clear()
	screen.ShowCursor(0, 0)
	screen.Fini()

	if timeLeft < 1 {
		fmt.Println("Ding - ding - time ran out. You are doomed to stay here forever ...")
	} else if fellInHole {
		fmt.Println("Yuck! You fell into one of your own holes.")
	} else if playerCaught {
		fmt.Println("You were caught and eaten by a zombie!")
	} else if exited {
		fmt.Println("Yay! You escaped the graveyard!")
	} else {
		fmt.Println("Goodbye.")
	}

	if maybePanic != nil {
		panic(maybePanic)
	}
}

func initGame(screen tcell.Screen) {
	screen.SetStyle(defStyle)
	screen.HideCursor()
	screen.Clear()

	for i := 0; i < ScreenW*ScreenH; i++ {
		playfield[i] = Space
	}

	for i := 0; i < ScreenW; i++ {
		playfield[i] = Wall
		playfield[((ScreenH-1)*ScreenW)+i] = Wall
	}

	for i := 0; i < ScreenH; i++ {
		playfield[i*ScreenW] = Wall
		playfield[(i*ScreenW)+ScreenW-1] = Wall
	}

	playfield[((ScreenH-2)*ScreenW)+ScreenW-1] = Space

	for i := 0; i < MaxGraves; {
		x := seededRand.Intn(ScreenW-2) + 1
		y := seededRand.Intn(ScreenH-2) + 1
		if playfield[(y*ScreenW)+x] == Grave {
			continue
		}

		playfield[(y*ScreenW)+x] = Grave
		i++
	}

	playerX = 1
	playerY = 1
	holesLeft = 3
	zombiesX[0] = ScreenW - 3
	zombiesX[1] = ScreenW - 4
	zombiesX[2] = ScreenW - 5
	zombiesY[0] = 2
	zombiesY[1] = 5
	zombiesY[2] = 8
	timeLeft = 60
	fellInHole = false
}

func movePlayer(dirX int, dirY int) {
	x := playerX + dirX
	y := playerY + dirY

	index := (y * ScreenW) + x
	if playfield[index] == Wall || playfield[index] == Grave {
		return
	}

	playerX = x
	playerY = y
}

func placeHole() {
	index := (playerY * ScreenW) + playerX

	if playfield[index] != Space || holesLeft < 1 {
		return
	}

	playfield[index] = Hole
	holesLeft--
	holePlaced = true
}

func moveZombies(move tcell.Key) {
	for z := 0; z < Zombies; z++ {
		if zombiesX[z] == -1 {
			continue
		}

		if move == tcell.KeyDown && moveableForZombie(z, 0, 1) {
			zombiesY[z]++
		} else if move == tcell.KeyUp && moveableForZombie(z, 0, -1) {
			zombiesY[z]--
		} else if move == tcell.KeyLeft {
			if playerX < zombiesX[z] && moveableForZombie(z, -1, 0) {
				zombiesX[z]--
			} else if playerX > zombiesX[z] && moveableForZombie(z, 1, 0) {
				zombiesX[z]++
			}
		}
	}
}

func moveableForZombie(zombie int, addX int, addY int) bool {
	x := zombiesX[zombie] + addX
	y := zombiesY[zombie] + addY
	ch := playfield[(y*ScreenW)+x]
	return ch == Space || ch == Hole
}

func drawText(screen tcell.Screen, x int, y int, style tcell.Style, text string) {
	for _, r := range []rune(text) {
		screen.SetContent(x, y, r, nil, style)
		x++
	}
}

func playerExited() bool {
	exited = playerX == ScreenW-1 && playerY == ScreenH-2
	return exited
}

func playerInHole() bool {
	index := (playerY * ScreenW) + playerX
	fellInHole = playfield[index] == Hole
	return fellInHole
}

func zombieInHole() {
	for z := 0; z < Zombies; z++ {
		if zombiesX[z] == -1 {
			continue
		}

		index := (zombiesY[z] * ScreenW) + zombiesX[z]
		if playfield[index] == Hole {
			zombiesX[z] = -1
		}
	}
}

func zombieCaughtPlayer() bool {
	for z := 0; z < Zombies; z++ {
		if zombiesX[z] == -1 {
			continue
		}

		if zombiesX[z] == playerX && zombiesY[z] == playerY {
			playerCaught = true
			return true
		}
	}

	return false
}

func update(screen tcell.Screen) {
	index := 0
	for row := 0; row < ScreenH; row++ {
		for col := 0; col < ScreenW; col++ {
			if playfield[index] == Space {
				screen.SetContent(col, row, ' ', nil, spaceStyle)
			} else if playfield[index] == Wall {
				screen.SetContent(col, row, '#', nil, wallStyle)
			} else if playfield[index] == Grave {
				screen.SetContent(col, row, '+', nil, graveStyle)
			} else if playfield[index] == Hole {
				screen.SetContent(col, row, 'O', nil, holeStyle)
			}
			index++
		}
	}

	for i := 0; i < Zombies; i++ {
		if zombiesX[i] == -1 {
			continue
		}

		screen.SetContent(zombiesX[i], zombiesY[i], '&', nil, playerStyle)
	}

	screen.SetContent(playerX, playerY, 'O', nil, playerStyle)

	sc := fmt.Sprintf("Holes left: %d  ", holesLeft)
	drawText(screen, 0, ScreenH+1, hudStyle, sc)
	sc = fmt.Sprintf("Time left: %d  ", timeLeft)
	drawText(screen, 0, ScreenH+2, hudStyle, sc)

	screen.Show()
}
