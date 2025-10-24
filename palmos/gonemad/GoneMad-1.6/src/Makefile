include ../TopMake.mk
byte95	=	$(shell printf "\x95")

all:	debug

debug:
	make -C build all APPNAME=$(APPNAME) DEBUG=1

initall:
	mkdir -p build
	cp Build.mak build/Makefile
	cat txt/KeywordHelp.txt | sed s?\\\\x95?$(byte95)?g > build/KeywordHelp.txt

clean:
	-make -C build clean
	rm -f txt/*~ *~
	rm -f $(APPNAME).prc

distclean:	clean
	rm -fr build

nondebug:	clean
	make -C build all APPNAME=$(APPNAME) DEBUG=0
