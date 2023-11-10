/*
 * DB.c
 */
#include "Include.h"

/* structs */
enum { FDFound, FDNotFound, FDError };

/* protos */
static UInt16 FindDatabase(DB *);
static void Panic(DB *);

/*
 * TaskCompare
 */
Int16
TaskCompare(void *r1, void *r2, Int16 u1, SortRecordInfoType *u2, SortRecordInfoType *u3, MemHandle u4)
{
	TTask *e1=r1, *e2=r2;
	Char *t1, *t2;
	UInt16 d1, d2;

	/* compare dates */
	d1=e1->date;
	d2=e2->date;
	if (d1<d2)
		return 1;

	if (d1>d2)
		return -1;

	/* then task names */
	t1=(Char *)e1+sizeof(TTask);
	t2=(Char *)e2+sizeof(TTask);
	return TxtGlueCaselessCompare(t1, StrLen(t1), &d1, t2, StrLen(t2), &d2);
}

/*
 * FindDatabase
 *
 * Search all cards for a given database.
 *
 *  -> dbo		Database object.
 *
 * Returns FDFound, FDNotFound or FDError.
 */
static UInt16
FindDatabase(DB *dbo)
{
	UInt16 cards=MemNumCards(), idx;

	for (idx=0; idx<cards; idx++) {
		if ((dbo->id=DmFindDatabase(idx, dbo->name))) {
			dbo->card=idx;
			return FDFound;
		}

		if (DmGetLastErr()!=dmErrCantFind)
			return FDError;
	}

	return FDNotFound;
}

/*
 * Panic
 *
 * Delete database & reset device.
 *
 *  -> dbo		Database object.
 */
static void
Panic(DB *dbo)
{
	DmDeleteDatabase(dbo->card, dbo->id);
	ErrFatalDisplay("(Panic) Major database error - resetting device.");
	SysReset();
}

/*
 * DBCreateDatabase
 *
 * Create the database.
 */
void
DBCreateDatabase(void)
{
	UInt16 dbVer=DBVERSION, attrs=dmHdrAttrBackup;
	DB dbo;
	MemHandle h;
	LocalID appID;
	AppInfoBlock *appInfo;

	DBInit(&dbo, DBNAME);
	if (FindDatabase(&dbo)!=FDNotFound)
		return;

	ErrFatalDisplayIf(DmCreateDatabase(0, dbo.name, CRID, DBTYPE, false)!=errNone, "(DBCreateDatabase) Cannot create database.");
	if (FindDatabase(&dbo)!=FDFound)
		Panic(&dbo);

	if (DBOpen(&dbo, dmModeReadWrite, true)==false)
		Panic(&dbo);

	if ((h=DmNewHandle(dbo.db, sizeof(AppInfoBlock)))==NULL)
		Panic(&dbo);

	appID=MemHandleToLocalID(h);
	DmSetDatabaseInfo(dbo.card, dbo.id, NULL, &attrs, &dbVer, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL);
	appInfo=(AppInfoBlock *)MemHandleLock(h);
	DmSet(appInfo, 0, sizeof(AppInfoBlock), 0);
	CategoryInitialize((AppInfoType *)appInfo, defaultCategories);
	MemPtrUnlock(appInfo);

	DBClose(&dbo);
}

/*
 * DBInit
 *
 * Initialize a database structure.
 *
 *  -> dbo		Database object.
 *  -> name		Database name.
 */
void
DBInit(DB *dbo, Char *name)
{
	MemSet(dbo, sizeof(DB), 0);
	StrNCopy(dbo->name, name, dmDBNameLength);
}

/*
 * DBOpen
 *
 * Open a database. Issue warning if unsuccessful.
 *
 *  -> dbo		Database object.
 *  -> mode		Open mode.
 *  -> verbose		Show Alert if open fails.
 *
 * Returns true if database opened successfully, false otherwise.
 */
Boolean
DBOpen(DB *dbo, UInt16 mode, Boolean verbose)
{
	UInt16 attrs=dmHdrAttrBackup;

	if (dbo->db)
		return true;	/* Already open */

	if (FindDatabase(dbo)==FDFound) {
		if ((dbo->db=DmOpenDatabase(dbo->card, dbo->id, mode))!=NULL) {
			DmSetDatabaseInfo(dbo->card, dbo->id, NULL, &attrs, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			return true;
		}
	}

	if (verbose==true)
		FrmAlert(aCannotOpenDatabase);

	return false;
}

/*
 * DBClose
 *
 * Close a database.
 *
 *  -> dbo		Database object.
 */
void
DBClose(DB *dbo)
{
	if (dbo->db)
		DmCloseDatabase(dbo->db);

	MemSet(dbo, sizeof(DB), 0);
}

/*
 * DBSetRecord
 *
 * Commit a task record to the database.
 *
 *  -> rIdx		Record index (or dmMaxRecordIndex).
 *  -> d		Date.
 *  -> t		Task txt.
 *  -> time		Time.
 *  -> cat		Category.
 */
void
DBSetRecord(UInt16 rIdx, UInt16 d, Char *t, UInt16 time, UInt16 cat)
{
	UInt32 recLen=sizeof(TTask), taskLen=StrLen(t)+1, offset=0, unique;
	UInt16 attrs;
	TTask *nil=0, *r;
	DmOpenRef db=dbData.db;
	MemHandle rh;

	recLen+=taskLen;

	if (rIdx==dmMaxRecordIndex)
		rh=DmNewRecord(db, &rIdx, recLen);
	else
		rh=DmResizeRecord(db, rIdx, recLen);

	ErrFatalDisplayIf(rh==NULL, "(DBSetRecord) Device is full.");

	r=MemHandleLock(rh);
	DmWrite(r, (UInt32)&nil->date, &d, sizeof(nil->date));
	DmWrite(r, (UInt32)&nil->time, &time, sizeof(nil->time));

	offset+=sizeof(TTask);
	DmWrite(r, offset, t, taskLen);

	MemHandleUnlock(rh);

	DmRecordInfo(db, rIdx, &attrs, &unique, NULL);
	attrs&=~dmRecAttrCategoryMask;
	attrs|=(cat|dmRecAttrDirty);
	DmSetRecordInfo(db, rIdx, &attrs, NULL);
	
	DmInsertionSort(db, TaskCompare, 0);
}

/*
 * DBGetRecord
 *
 * Retrieves a record at a given index.
 *
 *  -> recIdx		Record index.
 *
 * Returns MemHandle.
 */
MemHandle
DBGetRecord(UInt16 recIdx)
{
	MemHandle mh;

	mh=DmQueryRecord(dbData.db, recIdx);
	ErrFatalDisplayIf(mh==NULL, "(DBGetRecord) Cannot retrieve record.");
	
	return mh;
}

/*
 * DBIdToIndex
 *
 * Locate a record given the unique id.
 *
 *  -> id		Record Id.
 *
 * Returns index or dmMaxRecordIndex (error).
 */
UInt16
DBIdToIndex(UInt32 id)
{
	UInt16 idx;

	if (DmFindRecordByID(dbData.db, id, &idx)==errNone)
		return idx;

	return dmMaxRecordIndex;
}
