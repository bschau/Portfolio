/*
 * DB.c
 */
#include "Include.h"

/* structs */
enum { FDFound, FDNotFound, FDError };

/* protos */
static Int16 SFString(void *, void *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
static UInt16 FindDatabase(DB *);


/*
 * SFString
 */
static Int16
SFString(void *r1, void *r2, Int16 u1, SortRecordInfoPtr u2, SortRecordInfoPtr u3, MemHandle u4)
{
	return StrCaselessCompare(((HotSyncRecord *)r1)->appName, ((HotSyncRecord *)r2)->appName);
}

/*
 * FindDatabase
 *
 * Search all cards for a given database.
 *
 *  -> dbo	Database object.
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
 * DBCreateDatabase
 *
 * Create a database.
 */
void
DBCreateDatabase(void)
{
	UInt16 dbVer=DBVERSION, attrs=dmHdrAttrBackup;
	DB dbo;

	MemSet(&dbo, sizeof(DB), 0);
	StrNCopy(dbo.name, DBHotSyncIDs, dmDBNameLength);
	if (FindDatabase(&dbo)==FDNotFound) {
		ErrFatalDisplayIf(DmCreateDatabase(0, dbo.name, CRID, 'DATA', false)!=errNone, "(DBCreateDatabase) Cannot create database.");
	}

	if (DBOpen(&dbo, true)) {
		DmSetDatabaseInfo(dbo.card, dbo.id, NULL, &attrs, &dbVer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		DBClose(&dbo);
		return;
	}

	DmDeleteDatabase(dbo.card, dbo.id);
	ErrFatalDisplay("(DBCreateDatabase) Error while creating database.");
}

/*
 * DBOpen
 *
 * Open a database. Issue warning if unsuccesful (and if we were asked to do
 * so!).
 *
 *  -> dbo	Database object.
 *  -> verbose	Show alert if open fails.
 *
 * Returns true if database opened successfully, false otherwise.
 */
Boolean
DBOpen(DB *dbo, Boolean verbose)
{
	MemSet(dbo, sizeof(DB), 0);
	StrNCopy(dbo->name, DBHotSyncIDs, dmDBNameLength);
	if (FindDatabase(dbo)==FDFound) {
		if ((dbo->db=DmOpenDatabase(dbo->card, dbo->id, dmModeReadWrite))!=NULL)
			return true;
	}

	if (verbose==true)
		FrmCustomAlert(aCannotOpenDatabase, dbo->name, NULL, NULL);

	return false;
}

/*
 * DBClose
 *
 * Close a database.
 *
 *  -> dbo	Database object.
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
 * Commit a record to a database.
 *
 *  -> dbo		Database object.
 *  -> rIdx		Record index (or dmMaxRecordIndex).
 *  -> src		Source data.
 *
 * Returns record unique id.
 */
UInt32
DBSetRecord(DB *dbo, UInt16 rIdx, HotSyncRecord *src)
{
	DmOpenRef db=dbo->db;
	MemHandle rh;
	UInt32 uid;
	UInt16 attrs;

	if (rIdx==dmMaxRecordIndex)
		rh=DmNewRecord(db, &rIdx, sizeof(HotSyncRecord));
	else
		rh=DmResizeRecord(db, rIdx, sizeof(HotSyncRecord));

	ErrFatalDisplayIf(rh==NULL, "(DBSetTaskRecord) Device is full.");

	DmWrite(MemHandleLock(rh), 0, src, sizeof(HotSyncRecord));
	MemHandleUnlock(rh);

	DmRecordInfo(db, rIdx, &attrs, &uid, NULL);
	attrs&=~dmRecAttrCategoryMask;
	attrs|=dmUnfiledCategory;
	DmSetRecordInfo(db, rIdx, &attrs, NULL);
	DmReleaseRecord(db, rIdx, true);
	
	DmInsertionSort(db, SFString, 0);
	return uid;
}

/*
 * DBGetRecord
 *
 * Retrieves a record at a given index.
 *
 *  -> dbo		Database object.
 *  -> recIdx		Record Index.
 *
 *  Returns memhandle.
 */
MemHandle
DBGetRecord(DB *dbo, UInt16 recIdx)
{
	MemHandle mh;

	mh=DmQueryRecord(dbo->db, recIdx);
	ErrFatalDisplayIf(mh==NULL, "(DBGetRecord) Cannot retrieve record.");
	
	return mh;
}

/*
 * DBFindRecord
 *
 * Finds (if possible) a record.
 *
 *  -> dbo		Database object.
 *  -> src		Source data.
 *
 * Returns position or dmMaxRecordIndex if not found.
 */
UInt16
DBFindRecord(DB *dbo, HotSyncRecord *src)
{
	DmOpenRef db=dbo->db;
	UInt16 rIdx;
	MemHandle mh;
	HotSyncRecord *hsr;
	Int16 r;

	if ((rIdx=DmFindSortPosition(db, src, NULL, SFString, 0))==0)
		return dmMaxRecordIndex;

	if (rIdx==dmMaxRecordIndex) {
		if ((rIdx=DmNumRecordsInCategory(db, dmAllCategories))==1)
			return dmMaxRecordIndex;
	}

	rIdx--;
	mh=DBGetRecord(dbo, rIdx);
	hsr=(HotSyncRecord *)MemHandleLock(mh);

	r=StrCaselessCompare(src->appName, hsr->appName);
	MemHandleUnlock(mh);

	return (r) ? dmMaxRecordIndex : rIdx;
}

/*
 * DBPurge
 *
 * Remove hotsync names for non-existing applications.
 * 
 *  -> dbo		Database object.
 */
void
DBPurge(DB *dbo)
{
	UInt16 cards=MemNumCards(), rIdx=0, idx;
	DmOpenRef db=dbo->db;
	MemHandle mh;
	HotSyncRecord *hsr;
	Char buffer[dmDBNameLength+2];

	for (EVER) {
		if ((mh=DmQueryNextInCategory(db, &rIdx, dmAllCategories))==NULL)
			break;

		hsr=MemHandleLock(mh);
		StrNCopy(buffer, hsr->appName, dmDBNameLength);
		MemHandleUnlock(mh);

		rIdx++;
		for (idx=0; idx<cards; idx++) {
			if (!(DmFindDatabase(idx, hsr->appName))) {
				rIdx--;
				DmRemoveRecord(db, rIdx);
				break;
			}
		}
	}
}
