CC	=	gcc
DST	=	mkcossin
CFLAGS	=	-O2 -Wall
LIBS	=	-lm

$(DST):	$(DST).c
	$(CC) $(CFLAGS) -o $(DST) $(DST).c $(LIBS)
	./mkcossin > ../cossintab.c

clean:
	rm -f $(DST)
