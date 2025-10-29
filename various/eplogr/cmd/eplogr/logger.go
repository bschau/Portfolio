package main

import (
	"context"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"time"

	"github.com/google/uuid"
	"golang.ngrok.com/ngrok"
	"golang.ngrok.com/ngrok/config"
)

var eplogrRc EplogrRc
var noFileWriting bool
var path string

func Logger(rcFile EplogrRc, disableFile bool) {
	now := time.Now().Format(time.RFC3339)
	eplogrRc = rcFile
	noFileWriting = disableFile

	err := os.Chdir(eplogrRc.DestDir)
	if err != nil {
		log.Fatal(err)
	}

	p, err := os.Getwd()
	if err != nil {
		log.Fatal(err)
	}
	path = p

	tun := getTunnel(eplogrRc)
	fmt.Println(now, "Listening on:", tun.URL())
	fmt.Println(now, "Ready to serve!")
	err = http.Serve(tun, http.HandlerFunc(handler))
	if err != nil {
		log.Fatal(err)
	}
}

func getTunnel(eplogrRc EplogrRc) ngrok.Tunnel {
	tunnelConfig := getTunnelConfig(eplogrRc)

	if len(eplogrRc.AuthToken) == 0 {
		tun, err := ngrok.Listen(context.Background(), tunnelConfig)
		if err != nil {
			log.Fatal(err)
		}

		return tun
	}

	tun, err := ngrok.Listen(context.Background(),
		tunnelConfig,
		ngrok.WithAuthtoken(eplogrRc.AuthToken))
	if err != nil {
		log.Fatal(err)
	}

	return tun
}

func getTunnelConfig(eplogrRc EplogrRc) config.Tunnel {
	if len(eplogrRc.Domain) == 0 {
		return config.HTTPEndpoint()
	}

	return config.HTTPEndpoint(config.WithDomain(eplogrRc.Domain))
}

func handler(w http.ResponseWriter, r *http.Request) {
	now := time.Now().Format(time.RFC3339)
	if r.Method != http.MethodPost {
		fmt.Println(now, "GET requests not allowed")
		w.WriteHeader(http.StatusMethodNotAllowed)
		return
	}

	data, err := io.ReadAll(r.Body)
	if err != nil {
		fmt.Println(now, "Faild to read request:", err)
		return
	}

	len := len(data)
	log.Println("Received", len, "bytes")

	len = getBufferSize(len)
	data = data[0:len]
	log.Println(string(data))

	if !noFileWriting {
		if !writeToFile(now, data) {
			w.WriteHeader(http.StatusInternalServerError)
			return
		}
	}
	w.WriteHeader(http.StatusOK)
}

func getBufferSize(len int) int {
	if len > eplogrRc.MaxSize {
		return eplogrRc.MaxSize
	}

	return len
}

func writeToFile(now string, data []byte) bool {
	uuid := uuid.New()
	filename := uuid.String() + eplogrRc.Extension
	fullname := filepath.Join(path, filename)
	fmt.Println(now, "Writing to", fullname)
	out, err := os.Create(fullname)
	if err != nil {
		return false
	}
	defer out.Close()

	n, err := out.Write(data)
	if err != nil {
		fmt.Println(now, "Error", err, "while writing to", fullname, ":", err)
		return false
	}

	fmt.Println(now, "Wrote", n, "bytes to", fullname)
	return true
}
