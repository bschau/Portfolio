package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
)

func main() {
	configFile := flag.String("c", "", "Configuration file")
	help := flag.Bool("h", false, "Help")
	flag.Parse()

	if *help {
		Usage(0)
	}

	config := getGxdavrRcConfiguration(*configFile)
	config = verifyAndSetDefaults(config)

	if len(config.CalendarUrl) == 0 {
		fmt.Println("No CalendarUrl defined in config file - please check spelling")
	}

	if len(config.AddressbookUrl) == 0 {
		fmt.Println("No AddressbookUrl defined in config file - please check spelling")
	}

	port := ":" + strconv.Itoa(config.Port)
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		fmt.Println(r.Method, r.URL.Path)
		if len(r.Method) == 0 || r.Method == "GET" {
			if strings.HasPrefix(r.URL.Path, "/remote.php/webdav") {
				w.WriteHeader(200)
			}
		} else if r.Method == "PROPFIND" {
			if strings.HasPrefix(r.URL.Path, "/.well-known/caldav/") || strings.HasPrefix(r.URL.Path, "/remote.php/caldav/") {
				if len(config.CalendarUrl) > 0 {
					http.Redirect(w, r, config.CalendarUrl, http.StatusFound)
				}
			} else if strings.HasPrefix(r.URL.Path, "/.well-known/carddav/") || strings.HasPrefix(r.URL.Path, "/remote.php/carddav/") {
				if len(config.AddressbookUrl) > 0 {
					http.Redirect(w, r, config.AddressbookUrl, http.StatusFound)
				}
			}
		}
	})

	log.Fatal(http.ListenAndServe(port, nil))
}

func getGxdavrRcConfiguration(configFile string) GxdavrRc {
	result := GxdavrRc{}
	filename := getConfigurationFilename(configFile)
	_, err := os.Stat(filename)
	if err != nil {
		if os.IsNotExist(err) {
			panic("No such configuration file: " + filename)
		}
		panic("Cannot read from configuration file: " + filename)
	}
	raw, err := os.ReadFile(filename)
	if err != nil {
		panic("Cannot read from configuration file: " + filename)
	}

	err = json.Unmarshal(raw, &result)
	if err != nil {
		panic("Error while parsing configuration file: " + err.Error())
	}
	return result
}

func getConfigurationFilename(configFile string) string {
	if len(configFile) > 0 {
		return configFile
	}

	home, err := os.UserHomeDir()
	if err != nil {
		log.Fatal(err)
	}

	return home + "/.gxdavrrc"
}

func verifyAndSetDefaults(configFile GxdavrRc) GxdavrRc {
	if configFile.Port < 1 || configFile.Port > 65535 {
		fmt.Println("Configuration file Port invalid, setting to 8080")
		configFile.Port = 8080
	}

	return configFile
}
