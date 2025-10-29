include ../Common.mk
DST	=	glttf$(EXE)

all:	$(DST)

$(DST):	glttf.o
	$(CC) $(LDFLAGS) -o $(DST) glttf.o $(LIBS) -lgd -lpng -lz -ljpeg -lfreetype -lm

glttf.o:	glttf.c
	$(CC) $(CFLAGS) glttf.c -o glttf.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
