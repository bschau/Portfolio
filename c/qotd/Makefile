ifeq ($(OS),Windows_NT)
EXE	=	.exe
OSTYPE	=	-DWIN32
PREFIX	=	/c/opt/bin
CP	=	cp
endif

ifndef OSTYPE
EXE	=
UNAME	=	$(shell uname -s)
PREFIX	=	/usr/local/bin
CP	=	cp
ifeq ($(UNAME),Linux)
OSTYPE	=	-DLinux
endif
ifeq ($(UNAME),Darwin)
OSTYPE	=	-DMACOSX
endif
endif

DST	=	qotd$(EXE)
CFLAGS	+=	-O2 -Wall -I. $(OSTYPE) -c

all:	$(DST)

$(DST):	main.o qotd_file.o
	$(CC) $(LDFLAGS) -o $(DST) main.o qotd_file.o

main.o:	main.c
	$(CC) $(CFLAGS) main.c -o main.o

qotd_file.o:	qotd_file.c
	$(CC) $(CFLAGS) qotd_file.c -o qotd_file.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
