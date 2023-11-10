APPNAME		=	sit
LAUNCHERNAME	=	Secure It
APPID		=	"SeSt"
INCS		=	-I . -I ..
OBJS		=	blowfish.o editentry.o main.o md5.o pincode.o password.o preferences.o showentry.o start.o textual.o utils.o
PILRCARGS	=	-q $(INCS)
CFLAGS		=	-O2 -Wall -DCREATORID="\"$(APPID)\"" -DAPPVER=1 $(INCS) 
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORMS		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc

all:
	make -C build build

initall:
	cp Makefile build

clean:
	rm -f build/*.grc build/*.bin build/*.o build/*.ex $(DST) *~
	rm -f sit/*
	rm -f sit.zip

distclean:	clean
	-rm -f build/Makefile

build:	$(FORMS) $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n "$(LAUNCHERNAME)" *.bin *.grc

$(CDEGRC):	$(EXE)
	m68k-palmos-obj-res $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

%.o:	../%.c ../sit.h ../resources.h
	$(CC) $(CFLAGS) -c $<

tFRM03e8.bin:	../sit.rcp
	pilrc $(PILRCARGS) ../sit.rcp

public:	all
	cp $(DST) README CHANGELOG sit

zip:	all
	zip sit.zip CHANGELOG README sit.prc
