include ../Common.mk
DST	=	d2u$(EXE)

all:	$(DST)

$(DST):	d2u.o
	$(CC) $(LDFLAGS) -o $(DST) d2u.o

d2u.o:	d2u.c
	$(CC) $(CFLAGS) d2u.c -o d2u.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
