all:	clean build deploy

init:
	go get github.com/ungerik/go-rss

clean:
	-rm -f *~
	-go clean

build:
	go build

deploy:
	go install
