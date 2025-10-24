include ../../TopMake.mk
LAUNCHERNAME	=	Angles
APPID		=	GNWJ
INCS		=	-I ..
CFLAGS		=	-O2 -Wall $(INCS)
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
FORM		=	Angles
OBJS		=	AboutForm.o \
			CosSinTab.o \
			LowScores.o \
			PilotMain.o \
			Session.o \
			UI.o \
			Utils.o \
			fEnd.o \
			fGame.o \
			fMain.o
DEPS		=	../Resources.h ../Include.h ../$(FORM).rcp
ifeq ($(DEBUG),1)
OBJS		+=	Debug.o
CFLAGS		+=	-DDEBUG=1
endif

all:	$(FORM) $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc

$(CDEGRC):	$(EXE)
	$(OBJRES) $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

$(FORM):	../$(FORM).rcp $(DEPS)
	pilrc $(PILRCARGS) ../$(FORM).rcp

clean:
	rm -f *.grc *.bin *.o *.ex *.rcp
