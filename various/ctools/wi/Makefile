include ../Common.mk
DST	=	wi$(EXE)

all:	$(DST)

$(DST):	wi.o
	$(CC) $(LDFLAGS) -o $(DST) wi.o

wi.o:	wi.c
	$(CC) $(CFLAGS) wi.c -o wi.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
