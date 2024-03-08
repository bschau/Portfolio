include Common.mk
PREFIX	=	/usr/local/bin

all:	$(TGT)

install:	all

install-unix:	clean common-objs
	-make -C add install
	-make -C bdb install
	-make -C big install
	-make -C colconv install
	-make -C d2u install
	-make -C d64 install
	-make -C gencdoc install
	-make -C giftrans install
	-make -C glttf install
	-make -C jpginfo install
	-make -C lts install
	-make -C msync install
	-make -C remtilde install
	-make -C rsc install
	-make -C swffx install
	-make -C wallpaper-rotator install
	-make -C wi install
	-make -C zen install
	-make -C man install PREFIX=$(PREFIX)

build-win:	clean common-objs
	-make -C add
	-make -C big
	-make -C colconv
	-make -C d2u
	-make -C d64
	-make -C gencdoc
	-make -C lts
	-make -C remtilde
	-make -C setwallpaper
	-make -C swffx
	-make -C uname
	-make -C wi
	-make -C zen

common-objs:
	-make -C common clean all

archive:	distclean
	(cd ..; zip -r bstools.zip bstools/*)

clean:
	-make -C common clean
	-make -C man clean
	-make -C add clean
	-make -C bdb clean
	-make -C big clean
	-make -C colconv clean
	-make -C d2u clean
	-make -C d64 clean
	-make -C gencdoc clean
	-make -C giftrans clean
	-make -C glttf clean
	-make -C jpginfo clean
	-make -C lts clean
	-make -C msync clean
	-make -C remtilde clean
	-make -C rsc clean
	-make -C setwallpaper clean
	-make -C swffx clean
	-make -C uname clean
	-make -C wallpaper-rotator clean
	-make -C wi clean
	-make -C zen clean
	-$(RM) *~
	-$(RM) sed.script
	-$(RM) setup.iss
	-$(RM) bstools-*.exe

distclean:	clean
	-make -C msync distclean
