include ../Common.mk
DST	=	bdb$(EXE)

all:	$(DST)

$(DST):	bdb.o
	$(CC) $(LDFLAGS) -o $(DST) bdb.o $(LIBS) -lgdbm

bdb.o:	bdb.c
	$(CC) $(CFLAGS) bdb.c -o bdb.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
