include TopMake.mk

all:	precompile english danish

precompile:
	make -C src distclean
	make -C src initall
	mkdir -p release
	cp Docs/CHANGELOG.txt Docs/LICENSE.txt Docs/README.txt release

english:
	make -C src english
	cp src/$(APPNAME).prc release
	(cd release; zip -r ../$(APPNAME)-english.zip $(APPNAME).prc CHANGELOG.txt LICENSE.txt README.txt)

danish:
	make -C src danish
	cp src/$(APPNAME).prc release
	(cd release; zip -r ../$(APPNAME)-danish.zip $(APPNAME).prc CHANGELOG.txt LICENSE.txt README.txt)

clean:
	-make -C src distclean
	-find . -type f -name "*~" -exec rm {} \;
	rm -f $(APPNAME)-english.zip $(APPNAME)-danish.zip
	rm -fr release

distclean:	clean

archive:	distclean
	(cd ..; tar cjf $(APPNAME)-$(VERSION).tar.bz2 $(APPNAME)-$(VERSION))
