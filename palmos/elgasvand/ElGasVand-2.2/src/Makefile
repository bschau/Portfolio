include ../TopMake.mk

all:	debug

debug:
	make -C build all APPNAME=$(APPNAME) DEBUG=1 LANG=Danish

initall:
	mkdir -p build
	cp Build.mak build/Makefile

clean:
	-make -C build clean
	rm -f txt/*~ translations/*~ *~
	rm -f $(APPNAME).prc

distclean:	clean
	rm -fr build

nondebug:	clean
	make -C build all APPNAME=$(APPNAME) DEBUG=0

danish:
	make -C build all APPNAME=$(APPNAME) LANG=Danish

english:
	make -C build all APPNAME=$(APPNAME) LANG=English
