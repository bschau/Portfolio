package main

import (
	"fmt"
	"io/ioutil"
	"math/rand"
	"os"
	"path"
	"runtime"
	"strings"
	"time"
)

var content string

// QotdFind - locate Qotd file
func QotdFind() string {
	env := os.Getenv("QOTD_FILE")
	env = strings.TrimSpace(env)
	if len(env) > 0 {
		_, err := os.Stat(env)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}

		return env
	}

	home, err := os.UserHomeDir()
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	var file string
	if runtime.GOOS == "windows" {
		file = "_qotd.txt"
	} else {
		file = ".qotd.txt"
	}

	qotdFile := path.Join(home, file)
	_, err = os.Stat(qotdFile)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	return qotdFile
}

// QotdLoad - load quote file and create nodes
func QotdLoad(path string) {
	memory, err := ioutil.ReadFile(path)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	content = string(append(memory, '\n'))
}

// QotdPick - pick random quote
func QotdPick() string {
	source := rand.NewSource(time.Now().UnixNano())
	r := rand.New(source)

	length := len(content)
	index := r.Intn(length - 1)
	for ; content[index] == '%'; index = r.Intn(length - 1) {
	}

	start := index - 1
	for ; start >= 0; start-- {
		if content[start] != '%' {
			continue
		}

		if start > 0 && content[start-1] == '\n' {
			start++
			break
		}
	}

	if start < 0 {
		start = 0
	}

	end := index
	for ; end < length; end++ {
		if content[end] != '%' {
			continue
		}

		s := end + 1
		if content[s] == '\r' {
			s++
		}

		if content[s] == '\n' {
			break
		}
	}

	return strings.TrimSpace(content[start:end])
}

// QotdOutput - parse and output qotd
func QotdOutput(qotd string, powerShell bool) {
	inBold := false
	inItalic := false

	for i := 0; i < len(qotd); {
		c := qotd[i]
		i++
		if c == '\\' {
			fmt.Printf("%c", qotd[i])
			i++
		} else if c == '*' {
			if powerShell {
				continue
			}
			if inBold {
				fmt.Printf("\033[0m")
			} else {
				fmt.Printf("\033[1m")
			}
			inBold = !inBold
		} else if c == '_' {
			if powerShell {
				continue
			}
			if inItalic {
				fmt.Printf("\033[0m")
			} else {
				fmt.Printf("\033[7m")
			}
			inItalic = !inItalic
		} else if c == '#' {
			fmt.Printf(" ")
		} else {
			fmt.Printf("%c", c)
		}
	}
}
