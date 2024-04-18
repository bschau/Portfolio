CC	=	gcc
CFLAGS	=	-c -O2 -Wall
LDFLAGS	=
SOURCES	=	main.c json.c map_cities.c map_close.c map_remove_sparks.c map_terrainify.c map_utils.c
OBJECTS =	$(SOURCES:.c=.o)
DST	=	makemap

all:	run

run:	$(SOURCES) $(DST)
	./makemap ../level1.json
	convert map.pnm map.png
	open map.png

$(DST):	$(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) *~ $(DST)
	rm -f map.pnm map.png
	rm -f world.json
