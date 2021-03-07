package main

import (
	"flag"
	"fmt"
	"html"
	"io"
	"log"
	"net/http"
	"os"
	"strconv"
	"time"
)

func main() {
	help := flag.Bool("h", false, "Help")
	docRoot := flag.String("d", ".", "Document root")
	portNum := flag.Int("p", 8080, "Port number")
	debug := flag.Bool("D", false, "Debug")
	flag.Parse()

	if *help {
		usage(0)
	}

	fs := dotFileHidingFileSystem{http.Dir(*docRoot)}
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		past := time.Now().UTC().AddDate(-1, 0, 0).Format(http.TimeFormat)
		headers := w.Header()
		// Cannot set Last-Modified as it is handled by the FileServer
		headers.Set("Cache-Control", "no-store,max-age=0")
		headers.Set("Expires", past)

		if *debug {
			fmt.Println(html.EscapeString(r.URL.Path))
		}

		http.FileServer(fs).ServeHTTP(w, r)
	})
	port := ":" + strconv.Itoa(*portNum)
	if *debug {
		fmt.Println("Listening on port " + port)
	}
	log.Fatal(http.ListenAndServe(port, nil))
	os.Exit(0)
}

func usage(ec int) {
	doc := `Usage: sauk v1.2
sauk [OPTIONS]
[OPTIONS]
 -D              Enable debugging
 -d path         Document root
 -h              Help (this page)
 -p num          Listen on port number
`
	s := getStream(ec)
	fmt.Fprint(s, doc)
	os.Exit(ec)
}

func getStream(exitCode int) io.Writer {
	if exitCode != 0 {
		return os.Stderr
	}

	return os.Stdout
}
