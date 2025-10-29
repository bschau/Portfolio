ifeq ($(OS),Windows_NT)
PREFIX		=	c:\opt\bin
CP		=	cp
RM		=	rm -f
RMDIR		=	rm -fr
EXE		=	.exe
OSTYPE		=	-DWIN32
TGT		=	build-win
LIBS		=
BIGFNTGEN	=	bigfntgen$(EXE)
endif

ifndef OSTYPE
PREFIX		=	/usr/local/bin/
CP		=	cp
RM		=	rm -f
RMDIR		=	rm -fr
TGT		=	install-unix
LIBS		=	-L/usr/lib
EXE		=
BIGFNTGEN	=	./bigfntgen
UNAME		=	$(shell uname -s)
ifeq ($(UNAME),Linux)
OSTYPE		=	-DLinux
endif
ifeq ($(UNAME),Darwin)
OSTYPE		=	-DMACOSX
endif
endif

LDFLAGS		=
CC		=	gcc
CFLAGS		=	-O2 -Wall -I. -I../common $(OSTYPE) -c 
