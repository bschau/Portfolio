CC	=	cc
CFLAGS	=

mkstemp:	mkstemp.c
	$(CC) $(CFLAGS) -o mkstemp mkstemp.c

clean:
	rm -f mkstemp *~

install:
	strip mkstemp
	cp mkstemp /usr/bin
	chmod 755 /usr/bin/mkstemp
	chown root /usr/bin/mkstemp
	chown system /usr/bin/mkstemp
