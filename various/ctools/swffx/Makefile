include ../Common.mk
DST	=	swffx$(EXE)

all:	$(DST)

$(DST):	swffx.o
	$(CC) $(LDFLAGS) -o $(DST) swffx.o

swffx.o:	swffx.c
	$(CC) $(CFLAGS) swffx.c -o swffx.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
	-$(RM) ftb.html
	-$(RM) ftb.sc
	-$(RM) ftb.swf
	-$(RM) ota.html
	-$(RM) ota.sc
	-$(RM) ota.swf
