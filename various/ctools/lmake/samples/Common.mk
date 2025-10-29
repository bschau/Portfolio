CFLAGS	=	$(CFLAGS) -I ../../common

all:	initall
	lmake -C build -f ../LMakefile -DBUILD build

initall:
	test -d build || mkdir build

build:
	msg Building for $(OS)
	< $(SOURCES) > compile
	basename s $(SOURCES)
	$(CC) $(CFLAGS) -o ../$(DST) $(s).o

compile:
	depend $<.o ../$<.c $(DEPS) || $(CC) $(CFLAGS) -D$(OS) -c ../$<.c

Cclean:
	$(RM) *~
	$(RM) scripts$/*~
	if -d build
		$(RM) build$/*.o
	endif
	$(RM) $(DST)

Cdistclean:	Cclean
	if -d build
		$(RD) build
	endif
	if -d cdoc
		$(RD) cdoc
	endif

Cdoc:
	lcdoc -o cdoc $(SOURCES).c
