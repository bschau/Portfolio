include ../Common.mk
DST	=	rsc$(EXE)
CFLAGS	+=	-I../common
SOURCES =	cmd_ignore.o cmd_info.o cmd_init.o cmd_lock.o cmd_set.o cmd_sync.o cmd_unlock.o rsc.o utils.o
ifeq ($(OS),Windows_NT)
SOURCES	+=	fnmatch.o
endif

all:	$(DST)

$(DST):	$(SOURCES)
	$(CC) $(LDFLAGS) -o $(DST) $(SOURCES) ../common/strings.o ../common/stringbuilder.o

cmd_ignore.o:	cmd_ignore.c
	$(CC) $(CFLAGS) cmd_ignore.c -o cmd_ignore.o

cmd_info.o:	cmd_info.c
	$(CC) $(CFLAGS) cmd_info.c -o cmd_info.o

cmd_init.o:	cmd_init.c
	$(CC) $(CFLAGS) cmd_init.c -o cmd_init.o

cmd_lock.o:	cmd_lock.c
	$(CC) $(CFLAGS) cmd_lock.c -o cmd_lock.o

cmd_set.o:	cmd_set.c
	$(CC) $(CFLAGS) cmd_set.c -o cmd_set.o

cmd_sync.o:	cmd_sync.c
	$(CC) $(CFLAGS) cmd_sync.c -o cmd_sync.o

cmd_unlock.o:	cmd_unlock.c
	$(CC) $(CFLAGS) cmd_unlock.c -o cmd_unlock.o

fnmathc.o:	fnmatch.c
	$(CC) $(CFLAGS) fnmatch.c -o fnmatch.o

rsc.o:	rsc.c
	$(CC) $(CFLAGS) rsc.c -o rsc.o

utils.o:	utils.c
	$(CC) $(CFLAGS) utils.c -o utils.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
