DST	=	ttt

all:	clean
	go build -o $(DST)
	./$(DST) x h

.PHONY: clean
clean:
	go clean
	rm -f *~
