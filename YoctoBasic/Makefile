ifeq ($(OS),Windows_NT)
CC	=	tcc
CFLAGS	=	-c -Wall -DWIN32
EXE	=	.exe
else
CC	=	gcc
CFLAGS	=	-c -O2 -Wall -DLinux
EXE	=
endif
LDFLAGS	=
DEPS	=	Makefile yoctobasic.h
DST	=	ybas

all:	builddir $(DST)

builddir:
	test -d build || mkdir build

$(DST):	error.o expressions.o keywords.o main.o relop.o run.o stack.o utils.o
	$(CC) $(LDFLAGS) -o $(DST)$(EXE) build/*.o

error.o:	error.c $(DEPS)
	$(CC) $(CFLAGS) error.c -o build/error.o

expressions.o:	expressions.c $(DEPS)
	$(CC) $(CFLAGS) expressions.c -o build/expressions.o

keywords.o:	keywords.c $(DEPS)
	$(CC) $(CFLAGS) keywords.c -o build/keywords.o

main.o:	main.c $(DEPS)
	$(CC) $(CFLAGS) main.c -o build/main.o

relop.o:	relop.c $(DEPS)
	$(CC) $(CFLAGS) relop.c -o build/relop.o

run.o:	run.c $(DEPS)
	$(CC) $(CFLAGS) run.c -o build/run.o

stack.o:	stack.c $(DEPS)
	$(CC) $(CFLAGS) stack.c -o build/stack.o

utils.o:	utils.c $(DEPS)
	$(CC) $(CFLAGS) utils.c -o build/utils.o

.PHONY: clean
clean:
	rm -f build/*.o
	rm -f $(DST) *~

.PHONY: distclean
distclean:	clean
	rm -fr build
