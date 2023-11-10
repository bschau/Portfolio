include ../../TopMake.mk
APPNAME		=	Kasino
LAUNCHERNAME	=	Kasino
APPID		=	'kScc'
INCS		=	-I ..
CFLAGS		=	-O2 -Wall $(INCS)
LIBS		=	-lPalmOSGlue
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
OBJS		=	DB.o \
			PilotMain.o \
			UI.o \
			fMain.o \
			fNames.o \
			fNewGame.o \
			fRules.o \
			fScores.o \
			fWinner.o
DEPS		=	../Include.h ../Resources.h
ifeq ($(DEBUG),1)
OBJS		+=	Debug.o
CFLAGS		+=	-DDEBUG=1
endif

all:	$(FORM) $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc

$(CDEGRC):	$(EXE)
	$(OBJRES) $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

$(FORM):	$(APPNAME).rcp $(DEPS)
	pilrc $(PILRCARGS) $(APPNAME).rcp

$(APPNAME).rcp:	../FormsData.rcp ../translations/$(LANG).trl
	translate.awk ../translations/$(LANG).trl ../FormsData.rcp > $(APPNAME).rcp

clean:
	rm -f *.grc *.bin *.o *.ex *.rcp
