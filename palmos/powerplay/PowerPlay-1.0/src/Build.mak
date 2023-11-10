include ../../TopMake.mk
LAUNCHERNAME	=	PowerPlay
APPID		=	PPLY
INCS		=	-I ..
CFLAGS		=	-O2 -Wall $(INCS) -DCRID=\'$(APPID)\'
LIBS		=	
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
OBJS		=	DB.o \
			PilotMain.o \
			Session.o \
			UI.o \
			fAbout.o \
			fEnd.o \
			fGame.o \
			fMain.o \
			fPreferences.o
DEPS		=	../Resources.h ../Include.h
ifeq ($(DEBUG),1)
OBJS		+=	Debug.o
CFLAGS		+=	-DDEBUG=1
ifdef SYS
CFLAGS		+=	-DSYS=$(SYS)
else
ifeq ($(shell uname -s),Linux)
CFLAGS		+=	-DSYS=0
else
CFLAGS		+=	-DSYS=1
endif
endif
endif

all:	$(CDEGRC) $(FORM)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n 'PowerPlay' *.bin *.grc

$(CDEGRC):	$(EXE)
	$(OBJRES) $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

$(FORM):	../FormsData.rcp $(DEPS)
	pilrc $(PILRCARGS) ../FormsData.rcp

clean:
	rm -f *.grc *.bin *.o *.ex *.rcp
