APPNAME		=	bmilog
LAUNCHERNAME	=	BMILog
APPID		=	BgSc
INCS		=	-I ../include
CFLAGS		=	-O2 -Wall $(INCS)
LIBS		=	-lPalmOSGlue
PILRCARGS	=	-q -I .. $(INCS) -allowEditID
DST		=	$(APPNAME).prc
EXE		=	$(APPNAME).ex
FORM		=	tFRM03e8.bin
CDEGRC		=	code0000.$(EXE).grc
CC		=	m68k-palmos-gcc
OBJS		=	about.o \
			bmi.o \
			bmilog.o \
			conversion.o \
			db.o \
			editperson.o \
			find.o \
			history.o \
			main.o \
			palmprint.o \
			preferences.o \
			query.o \
			sqrt.o \
			ui.o \
			utils.o
DEPS		=	../include/bmilog.h ../include/constants.h ../include/protos.h ../include/resources.h ../include/structures.h
ifeq ($(DEBUG),1)
OBJS		+=	debug.o
CFLAGS		+=	-DDEBUG=1
endif

OBJRES		=	m68k-palmos-obj-res

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

$(APPNAME).rcp:	../formsdata.rcp ../translations/$(LANG).trl
	translate.awk ../translations/$(LANG).trl ../formsdata.rcp > $(APPNAME).rcp

clean:
	rm -f *.grc *.bin *.o *.ex *.rcp
