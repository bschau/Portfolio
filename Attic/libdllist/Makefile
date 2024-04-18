CC		= gcc
CFLAGS		= -O2 -Wall -shared -fPIC -DVERSION="\"1\"" -DREVISION="\"0\""
OBJS		= dllist.o
DST		= libdllist.so
DEPS		= dllist.h Makefile

all:	$(DST) testprog

$(DST):		$(DEPS) $(OBJS)
	$(CC) $(CFLAGS) -o $(DST) $(OBJS)

testprog:	testprog.c
	$(CC) -O2 -Wall -L. -o testprog testprog.c -ldllist

clean:
	rm -f $(DST) $(OBJS) testprog *~

install:	$(DST)
	cp $(DST) /usr/lib
	cp dllist.h /usr/include
	ldconfig -v

dllist.o:	dllist.c
