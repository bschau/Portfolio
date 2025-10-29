include Version.mk
MLC	=	mlc
DST	=	Eyes
SRC	=	src/Eyes/*.java
OTAURL	=	http://www.schau.com/midlets/get.php?p=eye
EMUL	=	n6310i

all:	$(DST).jar
	$(EMUL) $(DST)

$(DST).jar:	$(SRC) Specfile
	mlc -F

clean:
	rm -f $(DST).jar $(DST).jad
	find . -type f -name "*~" -exec rm -f {} \;

distclean:	clean
	rm -fr $(DST)-$(VERSION)
	rm -f $(DST)-$(VERSION).zip $(DST).old

jar:	$(DST).jar

archive:	distclean
	(cd ..; tar cjf $(DST)-$(VERSION).tar.bz2 $(DST)-$(VERSION))

release:	distclean	jar
	mkdir $(DST)-$(VERSION)
	cp $(DST).jad $(DST).jar $(DST)-$(VERSION)
	zip $(DST)-$(VERSION).zip $(DST)-$(VERSION)/$(DST).jar $(DST)-$(VERSION)/$(DST).jad
	rm -fr $(DST)-$(VERSION)
	
ota:	distclean	jar
	mv $(DST).jad $(DST).old
	sed /MIDlet-Jar-URL/s!"$(DST).jar"!"$(OTAURL)"!g $(DST).old > $(DST).jad
	rm -f $(DST).old
