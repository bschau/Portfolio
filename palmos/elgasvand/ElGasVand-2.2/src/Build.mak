include ../../TopMake.mk
LAUNCHERNAME	=	El, gas, vand
APPID		=	EGVx
INCS		=	-I ..
CFLAGS		=	-O2 -Wall $(INCS) -DCRID=\'$(APPID)\'
LIBS		=	
PILRCARGS	=	-q $(INCS) -allowEditID -L $(LANG)
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
OBJS		=	DB.o \
			PilotMain.o \
			UI.o \
			fAbout.o \
			fEdit.o \
			fKeyboard.o \
			fMain.o \
			fStatistics.o
DEPS		=	../Resources.h ../Include.h
ifeq ($(DEBUG),1)
OBJS		+=	Debug.o
CFLAGS		+=	-DDEBUG=1
endif

all:	$(CDEGRC) cleanform $(FORM)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc

$(CDEGRC):	$(EXE)
	$(OBJRES) $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

cleanform:	../FormsData.rcp
	rm -f $(FORM)

$(FORM):
	pilrc $(PILRCARGS) ../FormsData.rcp

clean:
	rm -f *.grc *.bin *.o *.ex *.rcp
