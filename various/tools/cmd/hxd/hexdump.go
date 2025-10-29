package main

import (
	"fmt"
	"log"
	"os"
)

// HexDump - dump the given block
func HexDump(fp *os.File, start int64, end int64) {
	buffer := make([]byte, 16)
	for count := end - start; count > 0; {
		fmt.Printf("%08x: ", start)

		toRead := 16
		if count < 16 {
			toRead = int(count)
			buffer = make([]byte, count)
		}

		_, err := fp.ReadAt(buffer, start)
		if err != nil {
			log.Fatal(err)
		}

		idx := 0
		blanks := 40

		if toRead > 1 {
			for i := 0; i < toRead/2; i++ {
				fmt.Printf("%02x%02x ", buffer[idx], buffer[idx+1])
				idx += 2
				blanks -= 5
			}
		}

		if toRead%2 == 1 {
			fmt.Printf("%02x   ", buffer[idx])
			blanks -= 5
		}

		for i := 0; i < blanks; i++ {
			fmt.Printf(" ")
		}

		fmt.Printf(" ")
		for i := 0; i < toRead; i++ {
			if buffer[i] < 32 || buffer[i] > 126 {
				fmt.Printf(".")
			} else {
				fmt.Printf("%c", buffer[i])
			}
		}

		fmt.Println()
		start += int64(toRead)
		count -= int64(toRead)
	}
}
