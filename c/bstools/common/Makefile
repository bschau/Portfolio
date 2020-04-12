include ../Common.mk

all:	hashtable.o stringbuilder.o strings.o

hashtable.o:	hashtable.c
	$(CC) $(CFLAGS) hashtable.c -o hashtable.o

stringbuilder.o:	stringbuilder.c
	$(CC) $(CFLAGS) stringbuilder.c -o stringbuilder.o

strings.o:	strings.c
	$(CC) $(CFLAGS) strings.c -o strings.o

clean:
	-$(RM) *~
	-$(RM) *.o
