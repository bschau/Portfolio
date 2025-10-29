DEBUG		=	0
APPNAME		=	sit
LAUNCHERNAME	=	Secure It
APPID		=	'SeSt'
INCS		=	-I . -I ..
CFLAGS		=	-O2 -Wall -DAPPVER=2 $(INCS) 
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORMS		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJS		=	about.o \
			blowfish.o \
			database.o \
			editentry.o \
			export.o \
			import.o \
			main.o \
			md5.o \
			password.o \
			pincode.o \
			preferences.o \
			reo.o \
			scrollbar.o \
			showentry.o \
			start.o \
			templateeditor.o \
			textual.o \
			utils.o \
			working.o
ifeq ($(DEBUG),1)
OBJS		+=	debug.o
CFLAGS		+=	-DDEBUG=1
endif

all:	english

english:
	make -C build LANGUAGE="english" build

french:
	make -C build LANGUAGE="french" build

german:
	make -C build LANGUAGE="german" build

initall:
	cp Makefile build

clean:
	rm -f build/*.grc build/*.bin build/*.o build/*.ex $(DST) *~
	rm -f sit.zip

distclean:	clean
	rm -f build/Makefile
	rm -f release/*

build:	$(FORMS) $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc

$(CDEGRC):	$(EXE)
	m68k-palmos-obj-res $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

%.o:	../%.c ../sit.h ../resources.h
	$(CC) $(CFLAGS) -c $<

tFRM03e8.bin:	../sit-$(LANGUAGE).rcp
	pilrc $(PILRCARGS) ../sit-$(LANGUAGE).rcp
