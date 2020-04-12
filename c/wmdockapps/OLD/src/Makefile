PREFIX	?=	/usr/local
DSTS	=	wmbattery wmbrian wmbutton wmcdrom wmclock wmmute wmshutdown wmwireless
CC	=	gcc
CFLAGS	=	-Wall -I. -I$(PREFIX)/include -O2
LDFLAGS	=
LIBS	=	-lX11 -lXpm -lwmdockapps
WLIBS	=	`get-wings-flags --libs`

all:	$(DSTS)

clean:
	-rm -f *.o
	-rm -f *~
	-rm -f $(DSTS)
	-rm -f bellconv bell.h

install:	clean $(DSTS)
	sudo install -s -t $(PREFIX)/bin wmbattery
	sudo install -s -t $(PREFIX)/bin wmbrian
	sudo install -s -t $(PREFIX)/bin wmbutton
	sudo install -s -t $(PREFIX)/bin wmcdrom
	sudo install -s -t $(PREFIX)/bin wmclock
	sudo install -s -t $(PREFIX)/bin wmmute
	sudo install -t $(PREFIX)/bin wmmute-helper
	sudo install -s -t $(PREFIX)/bin wmshutdown
	sudo install -s -t $(PREFIX)/bin wmwireless

################################################################################
#
#	WMBATTERY
#
wmbattery:	wmbattery.o
	$(CC) $(LDFLAGS) -o wmbattery wmbattery.o $(LIBS)

wmbattery.o:	wmbattery.c
	$(CC) $(CFLAGS) -c $<

################################################################################
#
#	WMBRIAN
#
wmbrian:	wmbrian.o
	$(CC) $(LDFLAGS) -o wmbrian wmbrian.o $(LIBS)

wmbrian.o:	wmbrian.c
	$(CC) $(CFLAGS) -c $<

################################################################################
#
#	WMBUTTON
#
wmbutton:	wmbutton.o
	$(CC) $(LDFLAGS) -o wmbutton wmbutton.o $(LIBS)

wmbutton.o:	wmbutton.c
	$(CC) $(CFLAGS) -c $<

################################################################################
#
#	WMCDROM
#
wmcdrom:	wmcdrom.o
	$(CC) $(LDFLAGS) -o wmcdrom wmcdrom.o $(LIBS)

wmcdrom.o:	wmcdrom.c
	$(CC) $(CFLAGS) -c $<

################################################################################
#
#	WMCLOCK
#
wmclock:	bellconv wmclock.o
	$(CC) $(LDFLAGS) -o wmclock wmclock.o -lm $(LIBS)

wmclock.o:	wmclock.c
	$(CC) $(CFLAGS) -c $<

bellconv:	bellconv.o
	$(CC) $(LDFLAGS) -o bellconv bellconv.o
	test -x bellconv && cat res/bell.wav | ./bellconv > bell.h

bellconv.o:	bellconv.c
	$(CC) $(CFLAGS) -c $<

################################################################################
#
#	WMMUTE
#
wmmute:	wmmute.o
	$(CC) $(LDFLAGS) -o wmmute wmmute.o $(LIBS)

wmmute.o:	wmmute.c
	$(CC) $(CFLAGS) -c $<

################################################################################
#
#	WMSHUTDOWN
#
wmshutdown:	wmshutdown.o
	$(CC) $(LDFLAGS) -o wmshutdown wmshutdown.o $(WLIBS) $(LIBS)

wmshutdown.o:	wmshutdown.c
	$(CC) $(CFLAGS) -c $<

################################################################################
#
#	WMWIRELESS
#
wmwireless:	wmwireless.o
	$(CC) $(LDFLAGS) -o wmwireless wmwireless.o $(LIBS)

wmwireless.o:	wmwireless.c
	$(CC) $(CFLAGS) -c $<
