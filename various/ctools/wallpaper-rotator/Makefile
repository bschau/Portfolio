include ../Common.mk
DST	=	wallpaper-rotator
CFLAGS	+=	-I../common $(shell pkg-config --cflags dconf)
LIBS	+=	$(shell pkg-config --libs dconf) -lproc

all:	$(DST)

$(DST):	daemon.o main.o wallpaper-rotator.o
	$(CC) $(LDFLAGS) -o $(DST) daemon.o main.o wallpaper-rotator.o $(LIBS)

daemon.o:	daemon.c
	$(CC) $(CFLAGS) daemon.c -o daemon.o

main.o:	main.c
	$(CC) $(CFLAGS) main.c -o main.o

wallpaper-rotator.o:	wallpaper-rotator.c
	$(CC) $(CFLAGS) wallpaper-rotator.c -o wallpaper-rotator.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
