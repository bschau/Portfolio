DST	=	iSecurViewer.exe
A	=	$(shell basename `pwd`)

all:
	make -C iSecurViewer DST=$(DST)

initall:
	cp Build.mak iSecurViewer/Makefile

clean:
	-make -C iSecurViewer clean DST=$(DST)
	-rm -f $(DST) *~

distclean:	clean
	-rm -f iSecurViewer/Makefile

archive:	distclean
	(cd ..; zip -r $(A).zip $(A)/)
