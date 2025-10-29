CC	=	gcc
CFLAGS	=	-O2 -Wall -I .. -DLINUX
BINDIR	=	/usr/local/bin
DEPS	=	../include.h ../Makefile
OBJS	=	comment.o \
		csvbuffer.o \
		datetime.o \
		fields.o \
		main.o \
		odebug.o \
		oexcel.o \
		ophp.o \
		ostdcsv.o \
		prc.o
DST	=	tttxexport

all:
	make -C build $(DST)

initall:
	(mkdir -p build; cp Makefile build)
	(cd ..; script/setver.sh)

$(DST):	$(OBJS)
	$(CC) $(CFLAGS) -o ../$(DST) $(OBJS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

clean:
	(cd build; rm -f $(OBJS))
	rm -f $(DST) *~ *.out

distclean: clean
	rm -fr build
	rm -f version.h

install:	all
	install -m 0755 -s $(DST) $(BINDIR)
