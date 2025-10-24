include TopMake.mk

all:
	make -C src distclean
	make -C src initall
	make -C src nondebug

clean:
	-make -C src distclean
	-find . -type f -name "*~" -exec rm {} \;
	rm -f $(APPNAME).prc

distclean:	clean

archive:	distclean
	(cd ..; tar cjf $(APPNAME)-$(VERSION).tar.bz2 $(APPNAME)-$(VERSION))
