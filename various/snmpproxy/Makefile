ifeq ($(shell basename `pwd`),build)
	include ../TopMake.mk
endif
DST		=	snmpproxy
BINDIR		=	/usr/local/bin
ETCDIR		=	/etc
OWNER		=	root
GROUP		=	root
RFCLIB		=	../rfclib/rfclib.a
OBJS		=	config.o dynbuf.o main.o oid.o proxy.o rget.o rnext.o trusted.o types.o utils.o
ifeq ($(shell uname -s),CYGWIN_NT-5.9)
	OBJS	+=	getopt.o
	CFLAGS	+=	-DWIN32
endif
ifeq ($(shell uname -s),Linux)
	CFLAGS	+=	-DLINUX
endif
ifeq ($(shell uname -s),HP-UX)
	CFLAGS	+=	-D_XOPEN_SOURCE_EXTENDED -DHPUX
endif

all:
	make -C build $(DST)

initall:
	(mkdir -p build; cp Makefile build)
	(mkdir -p rfclib; cp RfcLib.mk rfclib/Makefile)
	make -C rfclib

clean:
	rm -f build/*.o
	rm -f $(DST)
	rm -f $(DST).stackdump
	rm -f $(DST).gz
	find . -type f -name "*~" -exec rm -f {} \;

archive:	distclean
	(cd ..; rm -f snmpproxy.tar.gz; tar cvf snmpproxy.tar snmpproxy; gzip snmpproxy.tar)

bingz:		initall all
	strip $(DST)
	gzip $(DST)

distclean:	clean
	rm -fr build rfclib

install:	all
	install -g $(GROUP) -m 0755 -o $(OWNER) -s $(DST) $(BINDIR)
	install -g $(GROUP) -m 0644 -o $(OWNER) snmpproxy.conf-example $(ETCDIR)

$(DST):	$(OBJS)
	-kill `cat /tmp/snmpproxy1.pid`
	$(CC) $(CFLAGS) -o ../$(DST) $(OBJS) $(RFCLIB) $(LIBS)
	-rm -f /tmp/snmpproxy1.pid
	-rm -f /tmp/snmpproxy.out

%.o:	../%.c ../include.h
	$(CC) $(CFLAGS) -c $<

