A	=	$(shell basename `pwd`)
V	=	$(shell basename `pwd` | cut -f 2 -d -)
LANG	=	$(shell if test -s LANGUAGE; then cat LANGUAGE; fi)
PRGNAME	=	P4Remote
DST	=	$(PRGNAME)-$(LANG)
OTAURL	=	http://www.schau.com/midlets/get.php?p=p4r-$(LANG)
EMUL	=	n6230
J2MEC	=	/usr/local/j2mec/current/j2mec.jar
PROPS	=	-DProperties=languages/$(LANG).properties -DLANG=$(LANG)

all:	$(DST)
	$(EMUL) $(DST)

danish:
	echo "Danish" > LANGUAGE

english:
	echo "English" > LANGUAGE

initall:
	mkdir -p tmp

$(DST):
	java $(PROPS) -jar $(J2MEC) 

clean:
	rm -f $(DST).jar $(DST).jad
	find . -type f -name "*~" -exec rm -f {} \;

distclean:	clean
	rm -fr $(A)-$(LANG)
	rm -f $(A)-$(LANG).zip $(DST).old
	rm -fr tmp
	rm -f LANGUAGE
	rm -f $(PRGNAME)-*

archive:	distclean
	(cd ..; tar cjf $(A).tar.bz2 $(A))

release:	clean	initall	$(DST)
	mkdir $(A)-$(LANG)
	mv $(DST).jad $(DST).jar $(A)-$(LANG)
	zip $(A)-$(LANG).zip $(A)-$(LANG)/$(DST).jar $(A)-$(LANG)/$(DST).jad
	rm -fr $(A)-$(LANG)
	
ota:	clean	initall	$(DST)
	mv $(DST).jad $(DST).old
	sed /MIDlet-Jar-URL/s!"$(DST).jar"!"$(OTAURL)"!g $(DST).old > $(DST).jad
	rm -f $(DST).old

releases:	initall
	make danish
	make release
	make english
	make release

otas:	initall
	make danish
	make ota
	make english
	make ota
