include ../../TopMake.mk
LAUNCHERNAME	=	Dictate
APPID		=	dIcT
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
OBJS		=	Beam.o \
			DB.o \
			Exchange.o \
			IR.o \
			PilotMain.o \
			SortFunctions.o \
			String.o \
			UI.o \
			XRef.o \
			fAbout.o \
			fDictationDone.o \
			fGroup.o \
			fSaveScore.o \
			fSDictate.o \
			fSentence.o \
			fSentenceExport.o \
			fSentenceImport.o \
			fStudent.o \
			fTDictate.o \
			fTeacher.o \
			fViewAnswers.o \
			fWait.o
DEPS		=	../Resources.h ../Include.h ../FormsData.rcp
ifeq ($(DEBUG),1)
OBJS		+=	Debug.o
CFLAGS		+=	-DDEBUG=1
endif

all:	FormsData.rcp $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' *.bin *.grc

$(CDEGRC):	$(EXE)
	$(OBJRES) $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

FormsData.rcp:	../FormsData.rcp $(DEPS)
	pilrc $(PILRCARGS) ../FormsData.rcp

clean:
	rm -f *.grc *.bin *.o *.ex *.rcp
