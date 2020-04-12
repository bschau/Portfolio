DST	=	zen

all:	build

build:
	go build -o $(DST)

clean:
	-rm -f $(DST)
	-rm -f *~

install:	clean build
	strip $(DST)
	cp $(DST) $(GOPATH)
