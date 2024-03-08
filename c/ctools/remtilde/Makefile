include ../Common.mk
DST	=	remtilde$(EXE)

all:	$(DST)

$(DST):	remtilde.o
	$(CC) $(LDFLAGS) -o $(DST) remtilde.o

remtilde.o:	remtilde.c
	$(CC) $(CFLAGS) remtilde.c -o remtilde.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
