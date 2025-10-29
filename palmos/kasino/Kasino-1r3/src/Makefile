include ../TopMake.mk

all:	debug

debug:
	make -C build all APPNAME=$(APPNAME) DEBUG=1 LANG="english"

initall:
	mkdir -p build
	cp Build.mak build/Makefile

clean:
	-make -C build clean
	rm -f translations/*~
	rm -f *~
	rm -f $(APPNAME).prc

distclean:	clean
	rm -fr build

english:	clean
	make -C build all APPNAME=$(APPNAME) DEBUG=0 LANG="english"

danish:		clean
	make -C build all APPNAME=$(APPNAME) DEBUG=0 LANG="danish"
