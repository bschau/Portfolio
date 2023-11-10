VERSION	=	$(shell basename `pwd` | cut -f 2 -d '-')

all:
	@echo "Use:"
	@echo
	@echo "    make linux          - to compile and install linux version"

linux:
	chmod 755 script/setver.sh
	script/setver.sh
	make -C src distclean
	make -C src initall
	make -C src install

clean:
	make -C src distclean
	find . -type f -name "*~" -exec rm {} \;

distclean:	clean
	rm -f setup.iss

archive:	distclean
	(cd ..; tar cjf tttxexport-$(VERSION).tar.bz2 tttxexport-$(VERSION))
