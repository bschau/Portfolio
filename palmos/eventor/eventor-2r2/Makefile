APPNAME		=	Eventor
ZIP		=	zip
ZIPARGS		=	
ZIPARGSDIR	=	-r

all:	debug

debug:
	make -C build all APPNAME=$(APPNAME) LANG=danish DEBUG=1

initall:
	cp Build.mak build/Makefile

clean:
	make -C build clean
	rm -f translations/*~ *~
	rm -f docs/*~
	rm -f include/*~
	rm -f $(APPNAME).prc
	rm -f $(APPNAME).zip

distclean:	clean
	rm -f build/Makefile
	rm -f releases/*.zip
	rm -f CHANGELOG.txt LICENSE.txt README.txt 

danish:	
	make -C build all APPNAME=$(APPNAME) LANG=danish DEBUG=0

dk-release:	clean danish
	$(ZIP) $(ZIPARGS) $(APPNAME)-dk.zip $(APPNAME).prc CHANGELOG.txt LICENSE.txt README.txt
	mv $(APPNAME)-dk.zip releases

english:
	make -C build all APPNAME=$(APPNAME) LANG=english DEBUG=0

en-release:	clean english
	$(ZIP) $(ZIPARGS) $(APPNAME)-en.zip $(APPNAME).prc CHANGELOG.txt LICENSE.txt README.txt
	mv $(APPNAME)-en.zip releases

release:	initall clean
	rm -f releases/*.zip
	cp docs/CHANGELOG.txt .
	cp docs/LICENSE.txt .
	cp docs/README.txt .
	make dk-release
	make en-release
