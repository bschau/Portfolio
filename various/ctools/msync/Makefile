include ../Common.mk
DST	=	msync$(EXE)
CFLAGS	+=	-I../common
LDFLAGS +=	-L/usr/lib

all:	$(DST)

$(DST):	msync.o cmd_get.o cmd_ignore.o cmd_info.o cmd_init.o cmd_lock.o cmd_ls.o cmd_put.o cmd_rm.o cmd_sync.o cmd_unlock.o cmd_uptodate.o utils.o
	$(CC) $(LDFLAGS) -o $(DST) msync.o cmd_get.o cmd_ignore.o cmd_info.o cmd_init.o cmd_lock.o cmd_ls.o cmd_put.o cmd_rm.o cmd_sync.o cmd_unlock.o cmd_uptodate.o utils.o ../common/strings.o ../common/stringbuilder.o $(VERSION) -lcurl

msync.o:	msync.c
	$(CC) $(CFLAGS) msync.c -o msync.o

cmd_get.o:	cmd_get.c
	$(CC) $(CFLAGS) cmd_get.c -o cmd_get.o

cmd_ignore.o:	cmd_ignore.c
	$(CC) $(CFLAGS) cmd_ignore.c -o cmd_ignore.o

cmd_info.o:	cmd_info.c
	$(CC) $(CFLAGS) cmd_info.c -o cmd_info.o

cmd_init.o:	cmd_init.c
	$(CC) $(CFLAGS) cmd_init.c -o cmd_init.o

cmd_lock.o:	cmd_lock.c
	$(CC) $(CFLAGS) cmd_lock.c -o cmd_lock.o

cmd_ls.o:	cmd_ls.c
	$(CC) $(CFLAGS) cmd_ls.c -o cmd_ls.o

cmd_put.o:	cmd_put.c
	$(CC) $(CFLAGS) cmd_put.c -o cmd_put.o

cmd_rm.o:	cmd_rm.c
	$(CC) $(CFLAGS) cmd_rm.c -o cmd_rm.o

cmd_sync.o:	cmd_sync.c
	$(CC) $(CFLAGS) cmd_sync.c -o cmd_sync.o

cmd_unlock.o:	cmd_unlock.c
	$(CC) $(CFLAGS) cmd_unlock.c -o cmd_unlock.o

cmd_uptodate.o:	cmd_uptodate.c
	$(CC) $(CFLAGS) cmd_uptodate.c -o cmd_uptodate.o

utils.o:	utils.c
	$(CC) $(CFLAGS) utils.c -o utils.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o

distclean:	clean
	-$(RM) .MSYNC/rc
	-$(RM) .MSYNC/stamp
	-$(RM) .MSYNC/ignore
	-$(RMDIR) .MSYNC
