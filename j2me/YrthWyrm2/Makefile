VERSION	=	$(shell basename `pwd` | cut -f 2 -d -)
DST	=	YrthWyrm2
OTAURL	=	http://www.schau.com/midlets/get.php?p=yr2
PLT	=	$(shell uname -s)
ifeq ($(PLT),CYGWIN_NT-5.1)
J2MEC	=	"c:\Program Files\j2mec.jar"
PROPS	=	"-DProperties=Win32.properties;YrthWyrm2.properties"
EMUL	=	/cygdrive/c/WTK22/bin/emulator.exe -Xdescriptor $(DST).jad
else
J2MEC	=	/usr/local/j2mec/current/j2mec.jar
PROPS	=	-DProperties=Linux.properties:YrthWyrm2.properties
EMUL	=	/opt/WTK/wtk22/bin/emulator -Xdescriptor $(DST).jad
endif

all:	$(DST)
	$(EMUL)

initall:
	mkdir tmp

$(DST):
	java $(PROPS) -jar $(J2MEC) 

clean:
	rm -f $(DST).jar $(DST).jad
	find . -type f -name "*~" -exec rm -f {} \;

distclean:	clean
	rm -fr $(DST)-$(VERSION)
	rm -f $(DST)-$(VERSION).zip $(DST).old
	rm -fr tmp

archive:	distclean
	(cd ..; tar cjf $(DST)-$(VERSION).tar.bz2 $(DST)-$(VERSION))

release:	distclean	initall $(DST)
	mkdir $(DST)-$(VERSION)
	cp $(DST).jad $(DST).jar $(DST)-$(VERSION)
	zip $(DST)-$(VERSION).zip $(DST)-$(VERSION)/$(DST).jar $(DST)-$(VERSION)/$(DST).jad
	rm -fr $(DST)-$(VERSION)
	
ota:	distclean	initall	$(DST)
	mv $(DST).jad $(DST).old
	sed /MIDlet-Jar-URL/s!"$(DST).jar"!"$(OTAURL)"!g $(DST).old > $(DST).jad
	rm -f $(DST).old
