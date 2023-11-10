APPNAME		=	sepe
LAUNCHERNAME	=	SePe
APPID		=	'Sepe'
INCS		=	-I . -I ..
CFLAGS		=	-O2 -Wall -DAPPVER=1 $(INCS) 
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORMS		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJS		=	pilotmain.o

all:
	make -C build build

initall:
	cp Makefile build

clean:
	rm -f build/*.grc build/*.bin build/*.o build/*.ex $(DST) *~
	rm -f sepe.zip

distclean:	clean
	rm -f build/Makefile

build:	$(FORMS) $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc

$(CDEGRC):	$(EXE)
	m68k-palmos-obj-res $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

%.o:	../%.c ../resources.h
	$(CC) $(CFLAGS) -c $<

tFRM03e8.bin:	../sepe.rcp
	pilrc $(PILRCARGS) ../sepe.rcp

zip:	clean all
	zip sepe.zip sepe.prc
