/*
 * DB.c
 */
#include "Include.h"

/* FindDatabase return codes */
enum { FDFOUND, FDNOTFOUND, FDERROR };

/* protos */
static KasinoAppInfo *GetAppInfoBlock(void);
static Int16 RecordCompareFunction(UInt8 *, UInt8 *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
static UInt16 FindDatabase(Char *, UInt16 *, LocalID *);
static Boolean CreateDatabase(Char *);

/*
 * GetAppInfoBlock
 *
 * Returns locked pointer to AppInfoBlock (as KasinoAppInfo).
 */
static KasinoAppInfo *
GetAppInfoBlock(void)
{
	Err err;
	LocalID appID;

	err=DmDatabaseInfo(dbCard, dbLocalID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL);
	ErrFatalDisplayIf(err!=errNone, "(DBSetGame) Cannot get app info block.");

	return (KasinoAppInfo *)(MemLocalIDToLockedPtr(appID, dbCard));
}
	
/*
 * RecordCompareFunction
 *
 * Compare function for DBStoreRecord.
 *
 * See reference manual for arguments and return value.
 */
Int16
RecordCompareFunction(UInt8 *r1, UInt8 *r2, Int16 unused1, SortRecordInfoPtr unused2, SortRecordInfoPtr unused3, MemHandle unused4)
{
	UInt16 l1=StrLen(r1), l2=StrLen(r2), m1, m2;

	return TxtGlueCaselessCompare(r1, l1, &m1, r2, l2, &m2);
}

/*
 * FindDatabase
 *
 * Iterate through all memory cards and look for database.
 *
 *  -> name		Database name.
 *  -> card		Where to store card num.
 *  -> lid		Where to store local id.
 *
 * Returns FDFOUND, FDNOTFOUND or FDERROR.
 */
static UInt16
FindDatabase(Char *name, UInt16 *card, LocalID *lid)
{
	UInt16 numCards=MemNumCards(), idx;
	Err err;

	for (idx=0; idx<numCards; idx++) {
		if ((*lid=DmFindDatabase(idx, name))) {
			*card=idx;
			return FDFOUND;
		}

		if ((err=DmGetLastErr())!=dmErrCantFind)
			return FDERROR;
	}

	return FDNOTFOUND;
}

/*
 * CreateDatabase
 *
 * Creates the Kasino Data database.
 *
 *  -> dbName	Database name,
 *
 * Returns true or false.
 */
static Boolean
CreateDatabase(Char *dbName)
{
	UInt16 dbVer=DBVERSION, t;
	MemHandle h;
	LocalID appID;
	KasinoAppInfo *ai;

	if (DmCreateDatabase(0, dbName, CRID, DBTYPE, false)!=errNone)
		return false;

	t=FindDatabase(dbName, &dbCard, &dbLocalID);
	ErrFatalDisplayIf(t!=FDFOUND, "(CreateDatabase) FindDatabase Error.");

	dbH=DmOpenDatabase(dbCard, dbLocalID, dmModeReadWrite);
	ErrFatalDisplayIf(dbH==NULL, "(CreateDatabase) Cannot open database.");

	h=DmNewHandle(dbH, sizeof(KasinoAppInfo));
	ErrFatalDisplayIf(h==NULL, "(CreateDatabase) Cannot allocate new AppInfoBlock.");

	appID=MemHandleToLocalID(h);
	DmSetDatabaseInfo(dbCard, dbLocalID, NULL, NULL, &dbVer, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL);
	ai=(KasinoAppInfo *)MemHandleLock(h);
	DmSet(ai, 0, sizeof(KasinoAppInfo), 0);
	MemSet(&game, sizeof(KasinoGame), 0);
	DBSetGame();
	MemPtrUnlock(ai);

	return true;
}

/*
 * DBOpen
 *
 * Open database.
 *
 * Returns false if database is not found.
 */
Boolean
DBOpen(void)
{
	UInt16 attrs=dmHdrAttrBackup;
	Char dbName[dmDBNameLength];
	UInt16 t;
	Boolean dbErr;

	MemSet(dbName, dmDBNameLength, 0);
	StrNCopy(dbName, DBNAME, dmDBNameLength-1);

	t=FindDatabase(dbName, &dbCard, &dbLocalID);
	ErrFatalDisplayIf(t==FDERROR, "(DBOpen) FindDatabase Error.");
	if (t==FDFOUND) {
		dbH=DmOpenDatabase(dbCard, dbLocalID, dmModeReadWrite);
		ErrFatalDisplayIf(dbH==NULL, "(DBOpen) Cannot open database.");
		DBGetGame();
	} else {
		dbErr=CreateDatabase(dbName);
		ErrFatalDisplayIf(dbErr==false, "(DBOpen) Cannot create database.");
	}

	DmSetDatabaseInfo(dbCard, dbLocalID, NULL, &attrs, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	return true;
}

/*
 * DBClose
 *
 * Close database.
 */
void
DBClose(void)
{
	if (dbH!=NULL)
		DmCloseDatabase(dbH);
}

/*
 * DBSetGame
 *
 * Set game data in AppInfo block of database.
 */
void
DBSetGame(void)
{
	KasinoAppInfo *kai=GetAppInfoBlock();

	ErrFatalDisplayIf(kai==NULL, "(DBSetGame) AppInfoBlock not present.");

	DmWrite(kai, OffsetOf(KasinoAppInfo, game), &game, sizeof(KasinoGame));
	MemPtrUnlock(kai);
}

/*
 * DBGetGame
 *
 * Get game data from AppInfo block of database.
 */
void
DBGetGame(void)
{
	KasinoAppInfo *kai=GetAppInfoBlock();

	if (kai) {
		MemMove(&game, &kai->game, sizeof(game));
		MemPtrUnlock(kai);
	} else
		MemSet(&game, sizeof(KasinoGame), 0);
}

/*
 * DBAddName
 *
 * Adds a name to the database.
 *
 *  -> name	Name to add.
 */
void
DBAddName(Char *name)
{
	UInt16 nameLen=StrLen(name)+1, rIdx=dmMaxRecordIndex, idx, nl, m1, m2;
	MemHandle mh;
	Int16 scc;
	Char *n;

	idx=DmFindSortPosition(dbH, name, NULL, (DmComparF *)RecordCompareFunction, 0);
	if (idx)
		idx--;

	mh=DmQueryRecord(dbH, idx);
	if (mh) {
		n=MemHandleLock(mh);
		nl=StrLen(n);
		scc=TxtGlueCaselessCompare(name, nameLen, &m1, n, nl, &m2);
		MemHandleUnlock(mh);
		if (scc==0)
			return;	/* name is already there */
	}
	
	/* it is not critical that names are not added ...
	   Annoying, yes.  Critcal, no. */
	if ((mh=DmNewRecord(dbH, &rIdx, nameLen))==NULL)
		return;

	DmWrite(MemHandleLock(mh), 0, name, nameLen);
	MemHandleUnlock(mh);

	DmInsertionSort(dbH, (DmComparF *)RecordCompareFunction, 0);
}
