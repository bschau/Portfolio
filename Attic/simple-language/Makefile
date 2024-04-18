CC	=	gcc
CFLAGS	=	-c -O2 -Wall
DEPS	=	Makefile global.h
DST	=	sl

all:	builddir $(DST)

builddir:
	test -d build || mkdir build

$(DST):	build/emitter.o build/error.o build/init.o build/lexer.o build/main.o build/parser.o build/symbol.o
	$(CC) $(LDFLAGS) -o $(DST)$(EXE) build/*.o

build/emitter.o:	emitter.c $(DEPS)
	$(CC) $(CFLAGS) emitter.c -o build/emitter.o

build/error.o:	error.c $(DEPS)
	$(CC) $(CFLAGS) error.c -o build/error.o

build/init.o:	init.c $(DEPS)
	$(CC) $(CFLAGS) init.c -o build/init.o

build/lexer.o:	lexer.c $(DEPS)
	$(CC) $(CFLAGS) lexer.c -o build/lexer.o

build/main.o:	main.c $(DEPS)
	$(CC) $(CFLAGS) main.c -o build/main.o

build/parser.o:	parser.c $(DEPS)
	$(CC) $(CFLAGS) parser.c -o build/parser.o

build/symbol.o:	symbol.c $(DEPS)
	$(CC) $(CFLAGS) symbol.c -o build/symbol.o

.PHONY: clean
clean:
	rm -f build/*.o
	rm -f $(DST) *~

.PHONY: distclean
distclean:	clean
	rm -fr build
