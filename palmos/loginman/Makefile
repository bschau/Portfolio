DEBUG		=	0
APPNAME		=	loginman
LAUNCHERNAME	=	LoginMan
APPID		=	QALM
INCS		=	-I ..
OBJS		=	clipping.o \
			database.o \
			editform.o \
			loginman.o \
			mainform.o \
			protocol.o \
			url.o \
			utils.o \
			viewform.o
PILRCARGS	=	-q $(INCS) -allowEditID
CFLAGS		=	-O2 -Wall -DDEBUG=$(DEBUG) $(INCS) 
ifeq ($(DEBUG),1)
CFLAGS		+=	-g
endif
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORMS		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
GDB		=	m68k-palmos-gdb
ifeq ($(DEBUG),1)
OBJS		+=	debug.o
endif

all:
	make -C build build

initall:
	cp Makefile build

clean:
	rm -f build/*.grc build/*.bin build/*.o build/*.ex $(DST) *~
	rm -f $(APPNAME).zip

distclean:	clean
	rm -f build/Makefile
		
build:	$(FORMS) $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc

$(CDEGRC):	$(EXE)
	m68k-palmos-obj-res $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

%.o:	../%.c ../$(APPNAME).h ../resources.h
	$(CC) $(CFLAGS) -c $<

tFRM03e8.bin:	../$(APPNAME).rcp
	pilrc $(PILRCARGS) ../$(APPNAME).rcp

zip:	all
	zip $(APPNAME).zip CHANGELOG README $(APPNAME).prc

debug:
	@echo Connect by issuing   target pilot localhost:6414
	$(GDB) build/$(EXE)
