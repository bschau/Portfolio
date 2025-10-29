A	=	$(shell basename `pwd`)

all:
	@echo "Use make clean or make archive"

clean:
	rm -fr pfoc/pfoc/bin
	rm -fr pfoc/pfoc/obj
	rm -fr pfoi/pfoi/bin
	rm -fr pfoi/pfoi/obj
	make -C prc distclean
	-find . -type f -name "*~" -exec rm {} \;

archive:	clean
	(cd ..; tar cjf $(A).tar.bz2 $(A))
