include ../../TopMake.mk
LAUNCHERNAME	=	PluaED
APPID		=	PLED
INCS		=	-I ..
CFLAGS		=	-O2 -Wall $(INCS) -DCRID=\'$(APPID)\'
LIBS		=	
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
OBJS		=	DB.o \
			PilotMain.o \
			UI.o \
			fAbout.o \
			fEdit.o \
			fMain.o \
			fNew.o \
			fPreferences.o
DEPS		=	../Resources.h ../Include.h ../FormsData.rcp
ifeq ($(DEBUG),1)
OBJS		+=	Debug.o
CFLAGS		+=	-DDEBUG=1
endif

all:	$(CDEGRC) tFRM03e8.bin
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc

$(CDEGRC):	$(EXE)
	$(OBJRES) $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

tFRM03e8.bin:	../FormsData.rcp $(DEPS)
	pilrc $(PILRCARGS) ../FormsData.rcp

clean:
	rm -f *.grc *.bin *.o *.ex *.rcp
