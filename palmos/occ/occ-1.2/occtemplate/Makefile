APPNAME		=	occtemplate
LAUNCHERNAME	=	template
APPID		=	OccX
INCS		=	-I .. -I ../../common
OBJS		=	occtemplate.o
PILRCARGS	=	-q $(INCS)
CFLAGS		=	-O2 -Wall $(INCS) 
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
CDUMP		=	../../tools/cdump
DEPS		=	../../common/otresources.h
OCCMAKEDIR	=	../../occmake
OCCMANDIR	=	../../occman

all:
	make -C build build

initall:
	cp Makefile build

distclean:	clean
	-rm -f build/Makefile

clean:
	-rm -f build/*.grc build/*.bin build/*.o build/*.ex $(DST) *~

build:	pilrc $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc
	$(CDUMP) other code0000 code0000.$(APPNAME).ex.grc $(OCCMAKEDIR)/code0000.c
	$(CDUMP) other code0001 code0001.$(APPNAME).ex.grc $(OCCMAKEDIR)/code0001.c
	$(CDUMP) other data0000 data0000.$(APPNAME).ex.grc $(OCCMAKEDIR)/data0000.c
	$(CDUMP) other pref0000 pref0000.$(APPNAME).ex.grc $(OCCMAKEDIR)/pref0000.c
	$(CDUMP) other rloc0000 rloc0000.$(APPNAME).ex.grc $(OCCMAKEDIR)/rloc0000.c
	$(CDUMP) palm code0000 code0000.$(APPNAME).ex.grc $(OCCMANDIR)/code0000.c
	$(CDUMP) palm code0001 code0001.$(APPNAME).ex.grc $(OCCMANDIR)/code0001.c
	$(CDUMP) palm data0000 data0000.$(APPNAME).ex.grc $(OCCMANDIR)/data0000.c
	$(CDUMP) palm pref0000 pref0000.$(APPNAME).ex.grc $(OCCMANDIR)/pref0000.c
	$(CDUMP) palm rloc0000 rloc0000.$(APPNAME).ex.grc $(OCCMANDIR)/rloc0000.c

$(CDEGRC):	$(EXE)
	m68k-palmos-obj-res $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

pilrc:	../$(APPNAME).rcp
	pilrc $(PILRCARGS) ../$(APPNAME).rcp
