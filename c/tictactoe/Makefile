CC	=	gcc
CFLAGS	=	-c -O2 -Wall
DST	=	ttt

all:	$(DST)

$(DST):	ttt.o
	$(CC) $(LDFLAGS) -o $(DST) ttt.o

ttt.o:	ttt.c
	$(CC) $(CFLAGS) ttt.c -o ttt.o

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(DST) *~
