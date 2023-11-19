package main

import (
	"io/ioutil"
	"net/http"
	"strings"
)

// FetchURL fetch content on url
func FetchURL(url string) []byte {
	resp, err := http.Get(url)
	if err != nil {
		return nil
	}
	defer resp.Body.Close()
	if resp.StatusCode != 200 {
		return nil
	}

	data, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return nil
	}

	return data
}

// IndexAt Find first occurence of string starting at position
func IndexAt(src string, separator string, position int) int {
	idx := strings.Index(src[position:], separator)
	if idx == -1 {
		return -1
	}

	return idx + position
}
