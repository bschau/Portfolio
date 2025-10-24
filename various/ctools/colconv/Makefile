include ../Common.mk
DST	=	colconv$(EXE)

all:	$(DST)

$(DST):	colconv.o
	$(CC) $(LDFLAGS) -o $(DST) colconv.o

colconv.o:	colconv.c
	$(CC) $(CFLAGS) colconv.c -o colconv.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
