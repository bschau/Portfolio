PREFIX	?=	/usr/local
CC	=	gcc
CFLAGS	=	-Wall -I. -O2 -fPIC
DEPS	=	Makefile wmdockapps.h
WLIBS	=	`pkg-config WINGs --libs`
LIBS	=	-lX11 -lXpm $(WLIBS)

all:	builddir wmclock wmmute

builddir:
	test -d build || mkdir build

#
# wmdockapps
#
build/wmdockapps.o:	wmdockapps.h
	$(CC) $(CFLAGS) -c -o build/wmdockapps.o wmdockapps.c

#
# wmclock
#
wmclock:	build/bellconv build/wmdockapps.o build/wmclock.o
	$(CC) $(LDFLAGS) -o wmclock build/wmdockapps.o build/wmclock.o $(LIBS) -lm -lpulse-simple -lpulse

build/wmclock.o:	wmclock.c
	$(CC) $(CFLAGS) -c -o build/wmclock.o wmclock.c

build/bellconv:		build/bellconv.o
	test -x build/bellconv && cat res/bell.wav | build/bellconv > bell.h

build/bellconv.o:	bellconv.c
	$(CC) $(CFLAGS) -o build/bellconv bellconv.c

#
# wmmute
#
wmmute:	build/wmdockapps.o build/wmmute.o
	$(CC) $(LDFLAGS) -o wmmute build/wmdockapps.o build/wmmute.o $(LIBS) -lm -lpulse-simple -lpulse

build/wmmute.o:	wmmute.c
	$(CC) $(CFLAGS) -c -o build/wmmute.o wmmute.c

install:	clean all
	install -s -t $(PREFIX)/bin wmclock
	install -s -t $(PREFIX)/bin wmmute
	install -t $(PREFIX)/bin wmmute-helper
	install -t $(PREFIX)/man/man1 man/wmclock.1
	install -t $(PREFIX)/man/man1 man/wmmute.1
	install -t $(PREFIX)/man/man1 man/wmmute-helper.1
	
.PHONY: clean
clean:
	rm -f bell.h
	rm -f build/*.o
	rm -f *~
	rm -f wmclock wmmute

.PHONY: distclean
distclean:	clean
	rm -fr build
