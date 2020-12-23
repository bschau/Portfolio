package gobslib

import "strings"

// IndexAt Find first occurence of string starting at position
func IndexAt(src string, separator string, position int) int {
	idx := strings.Index(src[position:], separator)
	if idx == -1 {
		return -1
	}

	return idx + position
}
