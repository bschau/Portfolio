include ../TopMake.mk

all:	debug

debug:
	make -C build all APPNAME=$(APPNAME) DEBUG=1 SYS=1

initall:
	mkdir -p build
	cp Build.mak build/Makefile

clean:
	-make -C build clean
	rm -f txt/*~ *~
	rm -f $(APPNAME).prc

distclean:	clean
	rm -fr build

nondebug:	clean
	make -C build all APPNAME=$(APPNAME) DEBUG=0
