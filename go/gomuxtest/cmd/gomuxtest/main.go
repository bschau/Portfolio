package main

import (
	"html/template"
	"flag"
	"fmt"
	"log"
	"net/http"
	"strconv"

	"github.com/gorilla/mux"
)

var doc = `Usage: gomuxtest v1.0
gomuxtest [OPTIONS]
[OPTIONS]
  -D          Enable debugging.
  -h          Help (this page)
  -p port     Port number to use. Defaults to 8081.
`

type Todo struct {
	Title string
	Done  bool
}

type TodoPageData struct {
	PageTitle string
	Todos     []Todo
}

func main() {
	tmpl := template.Must(template.ParseFiles("assets/tmpl/layout.html"))
	debug := flag.Bool("D", false, "Debug")
	help := flag.Bool("h", false, "Help")
	portNum := flag.Int("p", 8081, "Port number")
	flag.Parse()

	r := mux.NewRouter()
	fs := http.FileServer(http.Dir("assets/"))
	r.PathPrefix("/static/").Handler(http.StripPrefix("/static/", fs))

	if *help {
		Usage(doc, 0)
	}
	fmt.Println("Running .. Debug is: " + strconv.FormatBool(*debug))

	r.HandleFunc("/list/{name}/todo/{item}", func(w http.ResponseWriter, r *http.Request) {
		vars := mux.Vars(r)
		data := TodoPageData{
			PageTitle: vars["name"],
			Todos: []Todo{
			{
				Title: vars["item"], Done: false},
			},
		}
		tmpl.Execute(w, data)
	})

	r.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		data := TodoPageData{
			PageTitle: "My TODO list",
			Todos: []Todo{
				{Title: "Task 1", Done: false},
				{Title: "Task 2", Done: true},
				{Title: "Task 3", Done: true},
			},
		}
		tmpl.Execute(w, data)
	})

	port := ":" + strconv.Itoa(*portNum)
	if *debug {
		fmt.Println("Listening on port" + port)
	}
	log.Fatal(http.ListenAndServe(port, r))
}
