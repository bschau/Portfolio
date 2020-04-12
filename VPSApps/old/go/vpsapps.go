package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strings"
)

// RCFile is the configuration file structure
type RCFile struct {
	Delivery     string
	MX           string
	MXPort       int
	From         string
	To           string
	LichessToken string
	MGDomain     string
	MGTo         string
	MGAuth       string
}

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

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "Usage: vpsapps chgvpnpwd |comics | lichess\n")
		os.Exit(1)
	}

	home := os.Getenv("HOME")
	file, _ := os.Open(home + "/.vpsappsrc")
	defer file.Close()

	decoder := json.NewDecoder(file)
	configuration := RCFile{}
	err := decoder.Decode(&configuration)
	if err != nil {
		log.Fatal("error:", err)
	}

	module := strings.ToLower(os.Args[1])
	if module == "comics" {
		comics(configuration)
	} else if module == "lichess" {
		lichess(configuration)
	} else if module == "chgvpnpwd" {
		chgvpnpwd(configuration)
	} else {
		fmt.Fprintf(os.Stderr, "Unknown module\n")
		os.Exit(1)
	}
}

// Deliver dispatches the mail by SMTP or Mailgun
func Deliver(configuration RCFile, subject string, body string) {
	if configuration.Delivery == "mg" {
		Mailgun(configuration, subject, body)
		return
	}

	SMTP(configuration, subject, body)
}
