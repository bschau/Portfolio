package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
)

// GetURL fetches content of URL
func GetURL(url string) string {
	resp, err := http.Get(url)
	if err != nil {
		return ""
	}
	defer resp.Body.Close()
	html, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return ""
	}
	return string(html)
}

func comicsHandler(w http.ResponseWriter, r *http.Request) {
	rss := comics(r.Host)
	w.Header().Set("Content-Type", "application/rss")
	fmt.Fprintf(w, rss)
}

func lichessHandler(w http.ResponseWriter, r *http.Request) {
	rss := lichess()
	w.Header().Set("Content-Type", "application/rss")
	fmt.Fprintf(w, rss)
}

func main() {
	port := getPort(os.Args)

	http.HandleFunc("/comics", comicsHandler)
	http.HandleFunc("/lichess", lichessHandler)

	http.ListenAndServe(":" + port, nil)
}

func getPort(args []string) string {
	if len(args) < 2 {
		return "54321"
	}

	return args[1]
}
