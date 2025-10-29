LAUNCHERNAME	=	Eventor
APPID		=	PERV
INCS		=	-I ../include
CFLAGS		=	-O2 -Wall $(INCS)
LIBS		=	-lPalmOSGlue
PILRCARGS	=	-q -I .. $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJRES		=	m68k-palmos-obj-res
OBJS		=	about.o \
			alarm.o \
			db.o \
			editform.o \
			eventor.o \
			find.o \
			main.o \
			palmprint.o \
			preferences.o \
			reo.o \
			timeselector.o \
			ui.o \
			utils.o \
			viewform.o
DEPS		=	../include/constants.h ../include/eventor.h ../include/protos.h ../include/resources.h ../include/structures.h
ifeq ($(DEBUG),1)
OBJS		+=	debug.o
CFLAGS		+=	-DDEBUG=1
endif

all:	$(FORM) $(CDEGRC)
	build-prc -t 'appl' -o ../$(DST) -c $(APPID) -n '$(LAUNCHERNAME)' --reset-after-install *.bin *.grc

$(CDEGRC):	$(EXE)
	$(OBJRES) $(EXE)

$(EXE):		$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o:	../%.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

$(FORM):	$(APPNAME).rcp $(DEPS)
	pilrc $(PILRCARGS) $(APPNAME).rcp

$(APPNAME).rcp:	../formsdata.rcp ../translations/$(LANG).trl
	translate.awk ../translations/$(LANG).trl ../formsdata.rcp > $(APPNAME).rcp

clean:
	rm -f *.grc *.bin *.o *.ex *.rcp
