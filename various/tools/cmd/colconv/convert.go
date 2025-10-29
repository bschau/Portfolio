package main

import (
	"strconv"
)

// ConvertFromHex converts from XXXXXX
func ConvertFromHex(src string) (int64, int64, int64) {
	v1, err := strconv.ParseInt(src[0:2], 16, 64)
	if err != nil {
		panic(err)
	}

	v2, err := strconv.ParseInt(src[2:4], 16, 64)
	if err != nil {
		panic(err)
	}

	v3, err := strconv.ParseInt(src[4:6], 16, 64)
	if err != nil {
		panic(err)
	}

	return v1, v2, v3
}

// ConvertFromOpenGL converts from 0.xx 0.xx 0.xx
func ConvertFromOpenGL(val1 string, val2 string, val3 string) (int64, int64, int64) {
	v1, err := strconv.ParseFloat(val1, 64)
	if err != nil {
		panic(err)
	}

	if v1 < 0.0 || v1 > 1.0 {
		panic("v1 must be 0.0 <= v1 <= 1.0")
	}

	v2, err := strconv.ParseFloat(val2, 64)
	if err != nil {
		panic(err)
	}

	if v2 < 0.0 || v2 > 1.0 {
		panic("v2 must be 0.0 <= v2 <= 1.0")
	}

	v3, err := strconv.ParseFloat(val3, 64)
	if err != nil {
		panic(err)
	}

	if v3 < 0.0 || v3 > 1.0 {
		panic("v3 must be 0.0 <= v3 <= 1.0")
	}

	return int64(v1 * 255), int64(v2 * 255), int64(v3 * 255)
}

// ConvertFromInt converts from x x x
func ConvertFromInt(val1 string, val2 string, val3 string) (int64, int64, int64) {
	v1, err := strconv.ParseInt(val1, 10, 64)
	if err != nil {
		panic(err)
	}

	if v1 < 0 || v1 > 255 {
		panic("v1 must be 0 <= v1 <= 255")
	}

	v2, err := strconv.ParseInt(val2, 10, 64)
	if err != nil {
		panic(err)
	}

	if v2 < 0 || v2 > 255 {
		panic("v2 must be 0 <= v2 <= 255")
	}

	v3, err := strconv.ParseInt(val3, 10, 64)
	if err != nil {
		panic(err)
	}

	if v3 < 0 || v3 > 255 {
		panic("v3 must be 0 <= v3 <= 255")
	}

	return v1, v2, v3
}
