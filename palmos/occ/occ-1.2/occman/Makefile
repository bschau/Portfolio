APPNAME		=	occman
VERSION		=	1r2
OCCMAKE		=	../occmake/occmake
CDUMP		=	../tools/cdump

all:	english

initall:
	cp Build.mak build/Makefile

clean:
	-make -C build clean
	-rm -f *~ $(APPNAME).prc

distclean:	clean
	-rm -f demos/*.prc
	-rm -f build/Makefile
	-rm -f large.c small.c
	-rm -f code0000.c code0001.c data0000.c pref0000.c rloc0000.c

bitmaps:
	../tools/bdump palm large ../common/large.bmp large.c
	../tools/bdump palm small ../common/small.bmp small.c

demos:	initall
	rm -f demos/*.prc
	$(OCCMAKE) -e -n SAOcc1 demos/saocc1 http://satest-url1/
	$(OCCMAKE) -e -n SAOcc2 demos/saocc2 http://satest-url2/
	$(OCCMAKE) -e -n SAOcc3 demos/saocc3 http://satest-url3/
	$(OCCMAKE) -e -n SAOcc4 demos/saocc4 http://satest-url4/
	$(OCCMAKE) -e -n SAOcc5 demos/saocc5 http://satest-url5/
	$(OCCMAKE) -e -n SAOcc6 demos/saocc6 http://satest-url6/
	$(OCCMAKE) -e -n SAOcc7 demos/saocc7 http://satest-url7/
	$(OCCMAKE) -e -n SAOcc8 demos/saocc8 http://satest-url8/
	$(OCCMAKE) -n Occ1 demos/occ1 http://test-url1/
	$(OCCMAKE) -n Occ2 demos/occ2 http://test-url2/
	$(OCCMAKE) -n Occ3 demos/occ3 http://test-url3/
	$(OCCMAKE) -n Occ4 demos/occ4 http://test-url4/
	$(OCCMAKE) -n Occ5 demos/occ5 http://test-url5/
	$(OCCMAKE) -n Occ6 demos/occ6 http://test-url6/
	$(OCCMAKE) -n Occ7 demos/occ7 http://test-url7/
	$(OCCMAKE) -n Occ8 demos/occ8 http://test-url8/
	$(OCCMAKE) -n LongUrlOCC demos/luo http://alongurlsafdasdasdfkwefhiwehfwiefgusldifugsldifugsdfugsadlfugsdliufgwerifudkhjsadlgsdighsdfahsfiuwgefuiasdfipugefpusdfpasudgfpusagdfpusadgfiasdugfuasydfgasudyfgsaodufygasodfygasodfygsoadfygsodfygsdofygasodufygasodfygsadofygsadofygsadofygsadfoygsadfoywgefosyadgfoasdgfosadyfgasdfgysdfgsaodfyasdgfoyasdgfoasdfygsaodfygsaodfygsadofygasdofygsadofgasdfygasdofgyasdfgasodfygasodfgasodfysadgfosaydfgsodyfgsoadfgsaodfygasodfg

english:
	make -C build all APPNAME=$(APPNAME) LANG=english
