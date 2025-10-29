include ../../TopMake.mk
LAUNCHERNAME	=	Gone Mad!
APPID		=	GMAD
INCS		=	-I ..
CFLAGS		=	-O2 -Wall $(INCS)
LIBS		=	
PILRCARGS	=	-q $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
FORM		=	GoneMad
OBJS		=	DB.o \
			IR.o \
			PilotMain.o \
			SortFunctions.o \
			Story.o \
			String.o \
			UI.o \
			fAbout.o \
			fEditTemplate.o \
			fHelp.o \
			fKeywords.o \
			fMain.o \
			fMemoBrowser.o \
			fStory.o \
			fTemplateExport.o \
			fTemplateExportSingle.o \
			fTemplateImport.o
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
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

$(FORM):	../$(FORM).rcp $(DEPS)
	pilrc $(PILRCARGS) ../$(FORM).rcp

clean:
	rm -f *.grc *.bin *.o *.ex *.rcp
