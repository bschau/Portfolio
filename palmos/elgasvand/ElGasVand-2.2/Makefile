include TopMake.mk

all:	compile-dk

compile-dk:
	make -C src distclean
	make -C src initall
	make -C src nondebug LANG="danish"

compile-en:
	make -C src distclean
	make -C src initall
	make -C src nondebug LANG="english"

package-dk:	compile-dk
	mkdir -p release
	cp Docs/CHANGELOG.txt Docs/LICENSE.txt Docs/README.txt release
	cp src/$(APPNAME).prc release
	(cd release; zip -r ../$(APPNAME)-dk.zip $(APPNAME).prc CHANGELOG.txt LICENSE.txt README.txt)

package-en:	compile-en
	mkdir -p release
	cp Docs/CHANGELOG.txt Docs/LICENSE.txt Docs/README.txt release
	cp src/$(APPNAME).prc release
	(cd release; zip -r ../$(APPNAME)-en.zip $(APPNAME).prc CHANGELOG.txt LICENSE.txt README.txt)

clean:
	-make -C src distclean
	-find . -type f -name "*~" -exec rm {} \;
	rm -f $(APPNAME)-dk.zip $(APPNAME)-en.zip
	rm -fr release

distclean:	clean

archive:	distclean
	(cd ..; tar cjf $(APPNAME)-$(VERSION).tar.bz2 $(APPNAME)-$(VERSION))
