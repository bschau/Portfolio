include ../Common.mk
DST	=	d64$(EXE)

all:	$(DST)

$(DST):	d64.o
	$(CC) $(LDFLAGS) -o $(DST) d64.o

d64.o:	d64.c
	$(CC) $(CFLAGS) d64.c -o d64.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
	-$(RM) extract_test.t64
