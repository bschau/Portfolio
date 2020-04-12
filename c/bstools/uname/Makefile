include ../Common.mk
DST	=	uname$(EXE)

all:	$(DST)

$(DST):	uname.o
	$(CC) $(LDFLAGS) -o $(DST) uname.o

uname.o:	uname.c
	$(CC) $(CFLAGS) uname.c -o uname.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
