CC	=	gcc
CFLAGS	=	-c -O2 -Wall
LDFLAGS	=	
SOURCES	=	tblmkr.c
OBJECTS =	$(SOURCES:.c=.o)
DST	=	tblmkr

all: $(SOURCES) $(DST)

$(DST):	$(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) *~ $(DST)

table:	clean $(DST)
	./$(DST) ../src/data/exptable.bin
	make clean
