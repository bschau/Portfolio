INCS		=	-I ..
CFLAGS		=	-O2 -Wall $(INCS) 
CC		=	gcc

all:
	make -C build cdump
	make -C build bdump

initall:
	cp Makefile build

distclean:	clean
	-rm -f build/Makefile

clean:
	-rm -f build/*.o cdump.exe cdump bdump.exe bdump *~

cdump:	cdump.o
	$(CC) $(CFLAGS) -o ../cdump cdump.o

bdump:	bdump.o
	$(CC) $(CFLAGS) -o ../bdump bdump.o

%.o:	../%.c
	$(CC) $(CFLAGS) -c $<
