include ../../TopMake.mk
LAUNCHERNAME	=	LcmGcf
APPID		=	LCMx
INCS		=	-I ..
CFLAGS		=	-O2 -Wall $(INCS) -DCRID=\'$(APPID)\'
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
FORM		=	FormsData
OBJS		=	AboutForm.o \
			DB.o \
			IR.o \
			PilotMain.o \
			Session.o \
			UI.o \
			fClue.o \
			fGame.o \
			fMain.o \
			fPreferences.o \
			fQuizOver.o \
			fScores.o
DEPS		=	../Resources.h ../Include.h ../$(FORM).rcp
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
