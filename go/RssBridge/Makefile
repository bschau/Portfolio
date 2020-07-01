SRCS	=	comics.go main.go
DST	=	RssBridge

all:	clean $(DST)

$(DST):	$(SRCS)
	go build -o $(DST)

pi:	clean $(SRCS)
	env GOOS=linux GOARCH=arm go build
	scp $(DST) $(VPSAPPSACCOUNT):/home/bs/bin

clean:
	rm -f $(DST)
	-find . -type f -name "*~" -exec rm -f {} \;

install:	clean $(DST)
	strip $(DST)
	scp $(DST) $(VPSAPPSACCOUNT):/home/bs/bin

setup:
	go get github.com/gorilla/feeds
