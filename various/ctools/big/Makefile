include ../Common.mk
DST	=	big$(EXE)

all:	$(DST)

$(DST):	bigfntgen$(EXE) big.o
	$(CC) $(LDFLAGS) -o $(DST) big.o

big.o:	big.c
	$(CC) $(CFLAGS) big.c -o big.o

bigfntgen$(EXE):	bigfntgen.o
	$(CC) $(LDFLAGS) -o bigfntgen$(EXE) bigfntgen.o
	$(BIGFNTGEN) lat1_16.psf bigfont.h

bigfntgen.o:	bigfntgen.c
	$(CC) $(CFLAGS) bigfntgen.c -o bigfntgen.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
	-$(RM) bigfntgen$(EXE)
	-$(RM) bigfont.h
