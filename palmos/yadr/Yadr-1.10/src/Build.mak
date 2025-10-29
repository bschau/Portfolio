include ../../TopMake.mk
LAUNCHERNAME	=	Yadr
APPID		=	YADr
INCS		=	-I ..
CFLAGS		=	-O2 -Wall $(INCS) -DCRID=\'$(APPID)\'
LIBS		=	-lPalmOSGlue
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
OBJS		=	Bookmarks.o \
			DB.o \
			Doc.o \
			History.o \
			IR.o \
			PilotMain.o \
			StringBuffer.o \
			UI.o \
			fAbout.o \
			fBookmarks.o \
			fFind.o \
			fFormatting.o \
			fHistory.o \
			fMain.o \
			fPreferences.o \
			fReader.o
DEPS		=	../Resources.h ../Include.h
ifeq ($(DEBUG),1)
OBJS		+=	Debug.o
CFLAGS		+=	-DDEBUG=1
endif

all:	$(CDEGRC) $(FORM)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n 'Yadr' *.bin *.grc

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
