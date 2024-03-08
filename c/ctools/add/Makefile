include ../Common.mk
DST	=	add$(EXE)

all:	$(DST)

$(DST):	add.o
	$(CC) $(LDFLAGS) -o $(DST) add.o ../common/stringbuilder.o

add.o:	add.c
	$(CC) $(CFLAGS) add.c -o add.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
