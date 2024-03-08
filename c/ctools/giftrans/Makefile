include ../Common.mk
DST	=	giftrans$(EXE)

all:	$(DST)

$(DST):	giftrans.o
	$(CC) $(LDFLAGS) -o $(DST) giftrans.o $(LIBS) -lgd

giftrans.o:	giftrans.c
	$(CC) $(CFLAGS) giftrans.c -o giftrans.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
