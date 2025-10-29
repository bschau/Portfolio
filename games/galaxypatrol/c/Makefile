ifeq ($(DDST),)
DDST	=	/usr/local/bin
endif
OBJS	=	build/main.o
DST	=	galaxypatrol
CC	=	gcc
CFLAGS	=	-c -O2 -Wall
DEPS	=	Makefile
LIBS	=	-lncurses

all:	builddir $(DST)

builddir:
	test -d build || mkdir build

$(DST):	$(OBJS)
	$(CC) -o $(DST) build/*.o $(LIBS)

clean:
	rm -f build/*.o *~
	rm -f $(DST)

distclean:	clean
	rm -fr build

build/main.o:	main.c $(DEPS)
	$(CC) $(CFLAGS) main.c -o build/main.o

install:	builddir $(DST)
	sudo install -s -t /usr/local/bin $(DST)
