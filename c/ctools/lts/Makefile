include ../Common.mk
DST	=	lts$(EXE)
CFLAGS	+=	-I../common

all:	$(DST)

$(DST):	lts.o cleanup.o path.o synchronize.o utils.o walk_dst.o
	$(CC) $(LDFLAGS) -o $(DST) lts.o cleanup.o path.o synchronize.o utils.o walk_dst.o ../common/hashtable.o

lts.o:	lts.c
	$(CC) $(CFLAGS) lts.c -o lts.o

cleanup.o:	cleanup.c
	$(CC) $(CFLAGS) cleanup.c -o cleanup.o

path.o:	path.c
	$(CC) $(CFLAGS) path.c -o path.o

synchronize.o:	synchronize.c
	$(CC) $(CFLAGS) synchronize.c -o synchronize.o

utils.o:	utils.c
	$(CC) $(CFLAGS) utils.c -o utils.o

walk_dst.o:	walk_dst.c
	$(CC) $(CFLAGS) walk_dst.c -o walk_dst.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
