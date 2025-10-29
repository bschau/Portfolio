package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"os"
	"runtime"
	"strconv"
	"time"
)

var now string

func main() {
	now = time.Now().Format(time.RFC3339)
	configFile := flag.String("c", "", "Configuration file")
	disableFile := flag.Bool("D", false, "Disable file writing")
	domain := flag.String("d", "", "ngrok domain")
	extension := flag.String("e", "", "File-extension of received file")
	maxSize := flag.Int("m", 0, "Max file size")
	help := flag.Bool("h", false, "Help")
	authToken := flag.String("t", "", "Authentication token")
	flag.Parse()

	if *help {
		Usage(0)
	}

	config := getEplogrConfiguration(*configFile)
	mergeEnvironment(config)
	if len(*domain) > 0 {
		config.Domain = *domain
	}

	if len(*extension) > 0 {
		config.Extension = *extension
	}

	if len(*authToken) > 0 {
		config.AuthToken = *authToken
	}

	if *maxSize > 0 {
		config.MaxSize = *maxSize
	}

	if config.MaxSize < 1 {
		config.MaxSize = 10240
	}

	args := flag.Args()
	if len(args) > 0 {
		config.DestDir = args[0]
	}

	if len(config.DestDir) == 0 {
		config.DestDir = "."
	}

	Logger(config, *disableFile)
}

func getEplogrConfiguration(configFile string) EplogrRc {
	result := EplogrRc{}
	filename := getConfigurationFilename(configFile)
	fmt.Println(now, "Reading configuration file", filename)
	_, err := os.Stat(filename)
	if err != nil {
		if os.IsNotExist(err) {
			fmt.Println(now, "No such configuration file - using defaults")
			return result
		}
		log.Fatal(err)
	}
	raw, err := os.ReadFile(filename)
	if err != nil {
		log.Fatal(err)
	}

	err = json.Unmarshal(raw, &result)
	if err != nil {
		log.Fatal("Error while parsing config file:", err)
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

	if runtime.GOOS == "windows" {
		return home + "/_eplogrrc"
	}

	return home + "/.eplogrrc"
}

func mergeEnvironment(eplogrRc EplogrRc) {
	eplogrRc.Domain = getVar("DOMAIN", eplogrRc.Domain)
	eplogrRc.AuthToken = getVar("AUTHTOKEN", eplogrRc.AuthToken)
	eplogrRc.Extension = getVar("EXTENSION", eplogrRc.Extension)
	eplogrRc.DestDir = getVar("DESTDIR", eplogrRc.DestDir)
	maxSize := getVar("MAXSIZE", strconv.Itoa(eplogrRc.MaxSize))
	intMaxSize, err := strconv.Atoi(maxSize)
	if err != nil {
		log.Fatal(err)
	}

	eplogrRc.MaxSize = intMaxSize
}

func getVar(key string, current string) string {
	keyName := "EPLOGR_" + key
	val := os.Getenv(keyName)
	if len(val) > 0 {
		return val
	}

	return current
}
