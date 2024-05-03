package main

import (
	"io"
	"net/http"
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

	data, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil
	}

	return data
}
