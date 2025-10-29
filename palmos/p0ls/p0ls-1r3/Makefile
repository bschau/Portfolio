APPNAME		=	p0ls
LAUNCHERNAME	=	0L Systems
APPID		=	"P0LS"
INCS		=	-I . -I ..
OBJS		=	cossintab.o display.o main.o io.o prodrules.o start.o turtle.o utils.o working.o
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
	rm -f $(APPNAME).zip
	make -C mkcossin clean

distclean:	clean
	rm -f build/Makefile

build:	$(FORMS) $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n "$(LAUNCHERNAME)" *.bin *.grc

$(CDEGRC):	$(EXE)
	m68k-palmos-obj-res $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

%.o:	../%.c ../$(APPNAME).h ../resources.h
	$(CC) $(CFLAGS) -c $<

tFRM03e8.bin:	../$(APPNAME).rcp
	pilrc $(PILRCARGS) ../$(APPNAME).rcp

zip:	all
	zip $(APPNAME).zip CHANGELOG README p0ls.prc
