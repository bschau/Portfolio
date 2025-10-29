include ../Common.mk
DST	=	gencdoc$(EXE)

all:	$(DST)

$(DST):	gencdoc.o output.o parse.o utils.o
	$(CC) $(LDFLAGS) -o $(DST) gencdoc.o output.o parse.o utils.o ../common/stringbuilder.o

gencdoc.o:	gencdoc.c
	$(CC) $(CFLAGS) gencdoc.c -o gencdoc.o

output.o:	output.c
	$(CC) $(CFLAGS) output.c -o output.o

parse.o:	parse.c
	$(CC) $(CFLAGS) parse.c -o parse.o

utils.o:	utils.c
	$(CC) $(CFLAGS) utils.c -o utils.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
