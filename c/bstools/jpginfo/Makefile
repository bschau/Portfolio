include ../Common.mk
DST	=	jpginfo$(EXE)
LIBS	+=	-ljpeg

all:	$(DST)

$(DST):	jpginfo.o
	$(CC) $(LDFLAGS) -o $(DST) jpginfo.o $(LIBS)

jpginfo.o:	jpginfo.c
	$(CC) $(CFLAGS) jpginfo.c -o jpginfo.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
