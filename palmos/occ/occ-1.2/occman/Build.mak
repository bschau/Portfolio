DEBUG		=	0
APPNAME		=	occman
LAUNCHERNAME	=	OCCMan
APPID		=	'OccX'
INCS		=	-I . -I .. -I ../../common
CFLAGS		=	-O2 -Wall $(INCS) 
LIBS		=	#-lPalmOSGlue
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
OBJS		=	editform.o \
			mainform.o \
			occ.o \
			occman.o \
			prc.o \
			ui.o \
			viewform.o \
			xstuff.o
DEPS		=	../occman.h ../resources.h
ifeq ($(DEBUG),1)
CFLAGS		+=	-DDEBUG=1
OBJS		+=	debug.o
endif

all:	$(FORM) $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc

$(CDEGRC):	$(EXE)
	$(OBJRES) $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

$(FORM):	../$(APPNAME).rcp $(DEPS)
	pilrc $(PILRCARGS) ../$(APPNAME).rcp

clean:
	rm -f *.grc *.bin *.o *.ex
