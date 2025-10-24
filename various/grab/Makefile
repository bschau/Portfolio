CC	=	gcc
CFLAGS	=	-c -O2 -Wall
LIBS	=	-ljpeg
DEPS	=	Makefile include.h
DST	=	grab
QCC	=	@echo '    ' CC $@;
QLINK	=	@echo '    ' LINK $@;

all:	builddir $(DST)

builddir:
	@test -d build || mkdir build
	
$(DST):	build/main.o build/utils.o build/devinit.o build/framereaders.o
	$(QLINK)$(CC) $(LDFLAGS) -o $(DST) build/*.o $(LIBS)

build/main.o:	main.c $(DEPS)
	$(QCC)$(CC) $(CFLAGS) main.c -o build/main.o

build/utils.o:	utils.c $(DEPS)
	$(QCC)$(CC) $(CFLAGS) utils.c -o build/utils.o

build/devinit.o:	devinit.c $(DEPS)
	$(QCC)$(CC) $(CFLAGS) devinit.c -o build/devinit.o

build/framereaders.o:	framereaders.c $(DEPS)
	$(QCC)$(CC) $(CFLAGS) framereaders.c -o build/framereaders.o

.PHONY: clean
clean:
	rm -f build/*.o
	rm -f $(DST) *~

.PHONY: distclean
distclean:	clean
	rm -fr build

.PHONY:	archive
archive:	distclean
	(cd ..; tar czvf grab.tar.gz grab)
