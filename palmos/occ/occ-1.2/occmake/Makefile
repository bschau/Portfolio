INCS		=	-I .. -I ../../common
CFLAGS		=	-O2 -Wall $(INCS)
CDUMP		=	../../tools/cdump
OBJS		=	bitmap.o \
			main.o \
			prc.o \
			utils.o \
			xstuff.o
DST		=	occmake
CC		=	gcc
#
#	Detection
#
# Windows 2000
ifeq ($(shell uname -s),CYGWIN_NT-5.0)
	OBJS	+=	getopt.o
	CFLAGS	+=	-DWIN32
endif
# Windows XP
ifeq ($(shell uname -s),CYGWIN_NT-5.1)
	OBJS	+=	getopt.o
	CFLAGS	+=	-DWIN32
endif

#
#	Common rules
#
all:
	make -C build $(DST)

distclean:	clean
	-rm -f build/Makefile
	-rm -f large.c small.c resources.h
	-rm -f code0000.c code0001.c data0000.c pref0000.c rloc0000.c

clean:
	-rm -f build/*.o build/*.ex build/*.grc $(DST) $(DST).exe *~

initall:
	cp Makefile build

bitmaps:
	../tools/cdump other large ../common/large.bmp large.c
	../tools/cdump other small ../common/small.bmp small.c

$(DST):	$(OBJS)
	$(CC) $(CFLAGS) -o ../$(DST) $(OBJS)

%.o:	../%.c ../$(DST).h
	$(CC) $(CFLAGS) -c $<
