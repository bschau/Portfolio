include TopMake.mk

all:	compile package

compile:
	make -C src distclean
	make -C src initall
	make -C src nondebug

package:	compile
	mkdir -p release
	cp Docs/CHANGELOG.txt Docs/LICENSE.txt Docs/README.txt Docs/UserGuide.pdf release
	cp src/$(APPNAME).prc release
	(cd release; zip -r ../$(APPNAME).zip $(APPNAME).prc CHANGELOG.txt LICENSE.txt README.txt UserGuide.pdf)

clean:
	-make -C src distclean
	-find . -type f -name "*~" -exec rm {} \;
	rm -f $(APPNAME).zip
	rm -fr release

distclean:	clean

archive:	distclean
	(cd ..; tar cjf $(APPNAME)-$(VERSION).tar.bz2 $(APPNAME)-$(VERSION))
