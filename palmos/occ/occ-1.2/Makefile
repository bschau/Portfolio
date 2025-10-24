DSTL	=	occ-l
DSTW	=	occ-w
SOURCE	=	1.2

all:	tools occtemplate occmake occman

distclean:	clean
	-rm -rf $(DSTL) $(DSTW) $(DSTL).tar $(DSTL).tar.gz $(DSTW).zip 
	-rm -rf goccmake/goccmake
	-make -C occdemo clean
	
source: distclean
	(cd ..; tar czvf occ-$(SOURCE).tar.gz occ-$(SOURCE))

phony:
	date > builddate

linux:		distclean all
	mkdir -p $(DSTL)
	cp occmake/occmake $(DSTL)
	strip $(DSTL)/occmake
	chmod 755 $(DSTL)/occmake
	cp occman/occman.prc $(DSTL)
	cp README CHANGES $(DSTL)
	tar cvf $(DSTL).tar $(DSTL)
	gzip $(DSTL).tar
	
windows:	distclean all
	mkdir -p $(DSTW)
	cp occmake/occmake.exe $(DSTW)
	strip $(DSTW)/occmake.exe
	cp occman/occman.prc $(DSTW)
	cp README CHANGES $(DSTW)
	pkzip -r $(DSTW).zip $(DSTW)/*
	mv OCC-W.ZIP occ-w.zip
	
clean:
	-rm -f *~ builddate
	-make -C tools distclean
	-make -C occman distclean
	-make -C occtemplate distclean
	-make -C occmake distclean

tools:	phony
	make -C tools initall
	make -C tools all

occtemplate:	phony
	make -C occtemplate initall
	make -C occtemplate all

occmake:	phony
	make -C occmake initall
	make -C occmake bitmaps
	make -C occmake all

occman:		phony
	make -C occman initall
	make -C occman bitmaps
	make -C occman all
