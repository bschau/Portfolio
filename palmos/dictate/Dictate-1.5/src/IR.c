/*
 * IR.c
 */
#include "Include.h"

/* protos */
static Err Reader(void *, UInt32 *, void *);
static Boolean Deleter(const char *, UInt16, UInt16, LocalID, void *);

/*
 * Reader
 *
 * Receive beam data.
 */
static Err
Reader(void *src, UInt32 *len, void *sck)
{
	Err err;

	*len=ExgReceive((ExgSocketType *)sck, src, *len, &err);
	return err;
}

/*
 * Deleter
 *
 * Delete database ... no chance!
 */
static Boolean
Deleter(const char *name, UInt16 version, UInt16 card, LocalID id, void *u)
{
	return (DmDeleteDatabase(card, id)==errNone) ? true : false;
}

/*
 * IRRegister
 *
 * Setup so that we can receive databases with .dIcT extensions.
 */
void
IRRegister(void)
{
	UInt32 version=0;
	Err err;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
	if (version<sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0))
		return;

	if (version<sysMakeROMVersion(4, 0, 0, sysROMStageRelease, 0))
		err=ExgRegisterData((UInt32)CRID, exgRegExtensionID, "dIcT");
	else
		err=ExgRegisterDatatype((UInt32)CRID, exgRegExtensionID, "dIcT", "Dictation Sentences", 0);

	if (err!=errNone)
		FrmAlert(aIRCannotRegister);
}

/*
 * IRReceive
 *
 * Receive data.
 *
 *  -> sck		Socket.
 */
void
IRReceive(ExgSocketType *sck)
{
	Err err;
	LocalID dbID;
	Boolean resetBit;
	EventType ev;

	if (ExgAccept(sck)!=errNone) {
		FrmAlert(aIRSocketError);
		return;
	}

	err=ExgDBRead(Reader, Deleter, sck, &dbID, 0, &resetBit, false);
	ExgDisconnect(sck, err);
	
	FtrSet((UInt32)CRID, 0, 1);

	MemSet(&ev, sizeof(ev), 0);
	ev.eType=nilEvent;
	EvtAddEventToQueue(&ev);
}

/*
 * IRImport
 *
 * Import data (if possible ...)
 */
void
IRImport(void)
{
	UInt32 ftr=0;

	if (FtrGet((UInt32)CRID, 0, &ftr)==ftrErrNoSuchFeature || ftr==0)
		return;

	FtrUnregister((UInt32)CRID, 0);
	DBClear(&dbBeam);
	if (DBOpen(&dbBeam, dmModeReadOnly, true)==false)
		return;

	if (DmNumRecords(dbBeam.db)>0) {
		UInt16 n=ExchangeIRToStudent();
		Char buffer[16];

		StrPrintF(buffer, "%d", n);
		FrmCustomAlert(aIRImport, buffer, (n==1) ? " " : "s ", NULL);
		fStudentIR();
	}

	DBClose(&dbBeam);
	if (DBFindDatabase(&dbBeam)==FDFound)
		DmDeleteDatabase(dbBeam.card, dbBeam.id);
}
