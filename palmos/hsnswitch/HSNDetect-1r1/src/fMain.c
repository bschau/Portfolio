/*
 * fMain.c
 */
#include <DLServer.h>
#include "Include.h"

/* structs */
enum { FDFound, FDNotFound, FDError };

/* protos */
static UInt16 FindDatabase(Char *, UInt16 *, LocalID *);
static void Detect(void);
static void DBSet(DmOpenRef, UInt32, void *);

/*
 * FindDatabase
 *
 * Search all cards for a given database.
 *
 *  -> name		Database name.
 * <-  card		Where to store card no.#
 * <-  id		Where to store LocalID.
 *
 * Returns FDFound, FDNotFound or FDError.
 */
static UInt16
FindDatabase(Char *name, UInt16 *card, LocalID *id)
{
	UInt16 cards=MemNumCards(), idx;

	for (idx=0; idx<cards; idx++) {
		if ((*id=DmFindDatabase(idx, name))) {
			*card=idx;
			return FDFound;
		}

		if (DmGetLastErr()!=dmErrCantFind)
			return FDError;
	}

	return FDNotFound;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		Detect();
		return true;

	default:
		break;
	}

	return false;
}

/*
 * Center
 *
 * Write centered text.
 *
 *  -> y	Y position.
 *  -> str	String.
 */
static void
Center(UInt16 y, Char *str)
{
	Int16 len=StrLen(str), pW=160;
	Boolean trunc;

	FntCharsInWidth(str, &pW, &len, &trunc);
	WinDrawChars(str, len, (160-pW)/2, y);
}

/*
 * Detect
 *
 * Run detection.
 */
static void
Detect(void)
{
	UInt32 romVersion=0, slot;
	UInt16 dbVer=1, attrs=dmHdrAttrBackup, dbCard;
	LocalID dbId;
	Char buffer[32], hotSyncName[dlkUserNameBufSize];
	RectangleType r;
	DmOpenRef db, prefDb;
	MemHandle mh;
	Char *os;

	r.topLeft.x=0;
	r.topLeft.y=15;
	r.extent.x=160;
	r.extent.y=145;
	WinEraseRectangle(&r, 0);

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if (romVersion<sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0)) {
		Center(80, "Error: incompatible ROM.");
		return;
	}

	hotSyncName[0]='\x00';
	DlkGetSyncInfo(NULL, NULL, NULL, hotSyncName, NULL, NULL);
	if (!*hotSyncName) {
		Center(80, "Error: please HotSync first.");
		return;
	}

	switch (FindDatabase(DBNameDetect, &dbCard, &dbId)) {
	case FDError:
		Center(80, "Error: cannot search (1).");
		return;
	case FDFound:
		if (DmDeleteDatabase(dbCard, dbId)!=errNone) {
			Center(80, "Error: cannot delete.");
			return;
		}
	default:	/* FALL-THRU */
		break;
	}

	if (DmCreateDatabase(0, DBNameDetect, CRID, 'DATA', false)!=errNone) {
		Center(80, "Error: cannot create.");
		return;
	}

	if (FindDatabase(DBNameDetect, &dbCard, &dbId)!=FDFound) {
		Center(80, "Error: cannot search (2).");
		return;
	}

	if (DmSetDatabaseInfo(dbCard, dbId, NULL, &attrs, &dbVer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone) {
		Center(80, "Error: cannot set database info.");
		return;
	}

	if ((db=DmOpenDatabase(dbCard, dbId, dmModeReadWrite))==NULL) {
		Center(80, "Error: cannot open.");
		return;
	}

	if ((prefDb=PrefOpenPreferenceDB(false))==NULL) {
		DmCloseDatabase(db);
		Center(80, "Error: cannot open prefs db.");
		return;
	}
	
	DBSet(db, sizeof(romVersion), &romVersion);
	os=SysGetOSVersionString();
	DBSet(db, StrLen(os), os);
	MemPtrFree(os);

	DBSet(db, StrLen(hotSyncName), hotSyncName);

	Center(20, "Collecting data ...");

	r.topLeft.y=40;
	r.extent.y=FntCharHeight();

	for (slot=0; slot<65536; slot++) {
		StrPrintF(buffer, "Slot: %u", (UInt16)slot);
		WinEraseRectangle(&r, 0);
		Center(40, buffer);

		if ((mh=DmGet1Resource('psys', slot))!=NULL) {
			DBSet(db, sizeof(slot), &slot);
			DBSet(db, MemHandleSize(mh), MemHandleLock(mh));
			MemHandleUnlock(mh);
			DmReleaseResource(mh);
		}
	}

	DmCloseDatabase(prefDb);
	DmCloseDatabase(db);

	Center(60, "All data collected ...");
	Center(80, "Please HotSync now. Send the");
	Center(92, "\"HSND_HSNDetectData.pdb\" file");
	Center(104, "to:    brian@schau.com");
	Center(120, "Afterwards, feel free to");
	Center(132, "delete this application.");
	Center(148, "Thank you!");
}

/*
 * DBSet
 *
 * Add a record to the specified database.
 *
 *  -> db		Database.
 *  -> size		Record size.
 *  -> src		Record data.
 */
static void
DBSet(DmOpenRef db, UInt32 size, void *src)
{
	UInt16 rIdx=dmMaxRecordIndex, attrs;
	MemHandle rh;

	rh=DmNewRecord(db, &rIdx, size);
	ErrFatalDisplayIf(rh==NULL, "(DBSet) Device is full.");

	DmWrite(MemHandleLock(rh), 0, src, size);
	MemHandleUnlock(rh);

	DmRecordInfo(db, rIdx, &attrs, NULL, NULL);
	attrs&=dmAllRecAttrs;
	attrs|=dmUnfiledCategory;
	DmSetRecordInfo(db, rIdx, &attrs, NULL);
	DmReleaseRecord(db, rIdx, true);
}
