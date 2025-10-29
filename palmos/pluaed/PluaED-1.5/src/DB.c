/*
 * DB.c
 */
#include "Include.h"

/*
 * DBFindDatabase
 *
 * Search all cards for a given database.
 *
 *  -> dbo	Database object.
 *
 * Returns FDFound, FDNotFound or FDError.
 */
UInt16
DBFindDatabase(DB *dbo)
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
 * DBInit
 *
 * Initialize a database object.
 *
 *  -> dbo	Database object.
 *  -> name	Database name.
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
 * Open a database. Issue warning if unsuccesful (and if we were asked to do
 * so!).
 *
 *  -> dbo	Database object.
 *  -> mode	Open mode.
 *  -> verbose	Show alert if open fails.
 *
 * Returns true if database opened successfully, false otherwise.
 */
Boolean
DBOpen(DB *dbo, UInt16 mode, Boolean verbose)
{
	if (dbo->db)
		return true;	/* Already open */

	if (DBFindDatabase(dbo)==FDFound) {
		if ((dbo->db=DmOpenDatabase(dbo->card, dbo->id, mode))!=NULL)
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

	DBClear(dbo);
}

/*
 * DBClear
 *
 * Clears the database object - leaves the name intact.
 *
 *  -> dbo	Database object.
 */
void
DBClear(DB *dbo)
{
	dbo->db=NULL;
	dbo->id=0;
	dbo->card=0;
}

/*
 * DBSetRecord
 *
 * Commit a record to a database.
 *
 *  -> dbo		Database object.
 *  -> rIdx		Record index (or dmMaxRecordIndex).
 *  -> category		Desired category.
 *  -> src		Source data.
 *  -> srcLen		Source length.
 *  -> sort		Sort function.
 *
 * Returns record unique id.
 */
UInt32
DBSetRecord(DB *dbo, UInt16 rIdx, UInt16 category, void *src, UInt16 srcLen, DmComparF sort)
{
	DmOpenRef db=dbo->db;
	MemHandle rh;
	UInt32 uid;
	UInt16 attrs;

	if (rIdx==dmMaxRecordIndex)
		rh=DmNewRecord(db, &rIdx, srcLen);
	else
		rh=DmResizeRecord(db, rIdx, srcLen);

	ErrFatalDisplayIf(rh==NULL, "(DBSetRecord) Device is full.");

	DmWrite(MemHandleLock(rh), 0, src, srcLen);
	MemHandleUnlock(rh);

	DmRecordInfo(db, rIdx, &attrs, &uid, NULL);
	attrs&=~dmRecAttrCategoryMask;
	attrs|=category;
	DmSetRecordInfo(db, rIdx, &attrs, NULL);
	DmReleaseRecord(db, rIdx, true);
	
	if (sort)
		DmInsertionSort(db, sort, 0);

	return uid;
}

/*
 * DBGetRecord
 *
 * Retrieves a record at a given index.
 *
 *  -> dbo	Database object.
 *  -> recIdx	Record Index.
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
