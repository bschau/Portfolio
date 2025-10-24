CC	=	gcc
CFLAGS	=	-c -O2 -Wall
LDFLAGS	=
SOURCES	=	main.c
OBJECTS =	$(SOURCES:.c=.o)
DST	=	gcf

all: $(SOURCES) $(DST)

$(DST):	$(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) *~ $(DST)
