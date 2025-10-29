include ../Common.mk
DST	=	setwallpaper$(EXE)

all:	$(DST)

$(DST):	setwallpaper.o
	$(CC) $(LDFLAGS) -o $(DST) setwallpaper.o

setwallpaper.o:	setwallpaper.c
	$(CC) $(CFLAGS) setwallpaper.c -o setwallpaper.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
