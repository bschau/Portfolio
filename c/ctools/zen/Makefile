ifeq ($(OS),Windows_NT)
EXE	=	.exe
OSTYPE	=	-DWIN32
PREFIX	=	/c/opt/bin
CP	=	cp
endif

ifndef OSTYPE
EXE	=
UNAME	=	$(shell uname -s)
PREFIX	=	/usr/local/bin
CP	=	cp
ifeq ($(UNAME),Linux)
OSTYPE	=	-DLinux
endif
ifeq ($(UNAME),Darwin)
OSTYPE	=	-DMACOSX
endif
endif

DST	=	zen$(EXE)
CFLAGS	+=	-O2 -Wall -I. $(OSTYPE) -c

all:	$(DST)

$(DST):	cmd_add.o cmd_close.o cmd_count.o cmd_delete.o cmd_edit.o cmd_init.o cmd_list.o cmd_reopen.o cmd_status.o cmd_view.o main.o editor.o stringbuilder.o utils.o zen_file.o
	$(CC) $(LDFLAGS) -o $(DST) cmd_add.o cmd_close.o cmd_count.o cmd_delete.o cmd_edit.o cmd_init.o cmd_list.o cmd_reopen.o cmd_status.o cmd_view.o main.o editor.o stringbuilder.o utils.o zen_file.o

cmd_add.o:	cmd_add.c
	$(CC) $(CFLAGS) cmd_add.c -o cmd_add.o

cmd_close.o:	cmd_close.c
	$(CC) $(CFLAGS) cmd_close.c -o cmd_close.o

cmd_count.o:	cmd_count.c
	$(CC) $(CFLAGS) cmd_count.c -o cmd_count.o

cmd_delete.o:	cmd_delete.c
	$(CC) $(CFLAGS) cmd_delete.c -o cmd_delete.o

cmd_edit.o:	cmd_edit.c
	$(CC) $(CFLAGS) cmd_edit.c -o cmd_edit.o

cmd_init.o:	cmd_init.c
	$(CC) $(CFLAGS) cmd_init.c -o cmd_init.o

cmd_list.o:	cmd_list.c
	$(CC) $(CFLAGS) cmd_list.c -o cmd_list.o

cmd_reopen.o:	cmd_reopen.c
	$(CC) $(CFLAGS) cmd_reopen.c -o cmd_reopen.o

cmd_status.o:	cmd_status.c
	$(CC) $(CFLAGS) cmd_status.c -o cmd_status.o

cmd_view.o:	cmd_view.c
	$(CC) $(CFLAGS) cmd_view.c -o cmd_view.o

main.o:	main.c
	$(CC) $(CFLAGS) main.c -o main.o

editor.o:	editor.c
	$(CC) $(CFLAGS) editor.c -o editor.o

stringbuilder.o:	stringbuilder.c
	$(CC) $(CFLAGS) stringbuilder.c -o stringbuilder.o

utils.o:	utils.c
	$(CC) $(CFLAGS) utils.c -o utils.o

zen_file.o:	zen_file.c
	$(CC) $(CFLAGS) zen_file.c -o zen_file.o

install:	$(DST)
	$(CP) $(DST) $(PREFIX)

clean:
	-$(RM) $(DST)
	-$(RM) *~
	-$(RM) *.o
