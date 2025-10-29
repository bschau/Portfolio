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
			if (DmDatabaseInfo(idx, dbo->id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone)
				continue;

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
 *
 *  -> name	Database name.
 *  -> catInit	Category Init (or 0 for no appinfo/category).
 */
void
DBCreateDatabase(Char *name, UInt16 catInit)
{
	UInt16 dbVer=DBVERSION, attrs=dmHdrAttrBackup;
	MemHandle h;
	LocalID appID;
	DB dbo;
	AppInfoType *appInfo;

	DBInit(&dbo, name, NULL);
	switch (DBFindDatabase(&dbo)) {
	case FDFound:
		return;
	case FDNotFound:
		ErrFatalDisplayIf(DmCreateDatabase(0, dbo.name, CRID, 'DATA', false)!=errNone, "(DBCreateDatabase) Cannot create database.");
		break;
	default:
		ErrFatalDisplay("(DBCreateDatabase) Cannot search database list.");
		break;
	}

	if (DBOpen(&dbo, dmModeReadWrite, true)==true) {
		DmSetDatabaseInfo(dbo.card, dbo.id, NULL, &attrs, &dbVer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		if (!catInit) {
			DBClose(&dbo);
			return;
		}

		if ((h=DmNewHandle(dbo.db, sizeof(AppInfoType)))) {
			appID=MemHandleToLocalID(h);
			DmSetDatabaseInfo(dbo.card, dbo.id, NULL, &attrs, &dbVer, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL);
			appInfo=MemHandleLock(h);
			DmSet(appInfo, 0, sizeof(AppInfoType), 0);
			CategoryInitialize(appInfo, catInit);
			MemPtrUnlock(appInfo);
			DBClose(&dbo);
			return;
		}

		DBClose(&dbo);
	}

	DmDeleteDatabase(dbo.card, dbo.id);
	ErrFatalDisplay("(DBCreateDatabase) Error while creating database.");
}

/*
 * DBInit
 *
 * Initialize a database object.
 *
 *  -> dbo	Database object.
 *  -> name	Database name.
 *  -> path	Path to database (if on card). This ptr must be valid throughout
 *  		the lifetime of the DB object!
 */
void
DBInit(DB *dbo, Char *name, Char *path)
{
	MemSet(dbo, sizeof(DB), 0);
	StrNCopy(dbo->name, name, dmDBNameLength);
	dbo->path=path;
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
	if (dbo->isOpen)
		return true;	/* Already open */

	if (!dbo->path) {
		if (DBFindDatabase(dbo)==FDFound) {
			if ((dbo->db=DmOpenDatabase(dbo->card, dbo->id, mode))!=NULL) {
				dbo->isOpen=true;
				return true;
			}
		}
	} else {
		UInt32 v=vfsIteratorStart;

		if (VFSVolumeEnumerate(&dbo->card, &v)==errNone) {
			if (VFSFileOpen(dbo->card, dbo->path, vfsModeRead, &dbo->fileRef)==errNone) {
				dbo->isOpen=true;
				return true;
			}
		}
	}

	if (verbose==true)
		FrmCustomAlert(aCannotOpenDatabase, dbo->name ? dbo->name : dbo->path, NULL, NULL);

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

	if (dbo->fileRef)
		VFSFileClose(dbo->fileRef);

	if (dbo->path)
		MemPtrFree(dbo->path);

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
	dbo->path=NULL;
	dbo->id=0;
	dbo->card=0;
	dbo->fileRef=0;
	dbo->isOpen=false;
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
	MemHandle mh=DBGetRecordSilent(dbo, recIdx);

	ErrFatalDisplayIf(mh==NULL, "(DBGetRecord) Cannot retrieve record.");
	return mh;
}

/*
 * DBGetRecordSilent
 *
 * Retrieves a record at a given index - this may return NULL.
 *
 *  -> dbo	Database object.
 *  -> recIdx	Record Index.
 *
 *  Returns memhandle or NULL if record was not found.
 */
MemHandle
DBGetRecordSilent(DB *dbo, UInt16 recIdx)
{
	MemHandle mh=NULL;

	if (!dbo->path) {
		MemHandle rh;
		UInt32 size;

		if ((rh=DmQueryRecord(dbo->db, recIdx))==NULL)
			return NULL;

		size=MemHandleSize(rh);
		if ((mh=MemHandleNew(size))!=NULL) {
			MemMove(MemHandleLock(mh), MemHandleLock(rh), size);
			MemHandleUnlock(rh);
			MemHandleUnlock(mh);
		}
	} else {
		UInt32 startOfRecord, endOfRecord, chunkSize, read;
		UInt16 cnt;
		UInt8 recEntry[8];

		if (VFSFileSeek(dbo->fileRef, vfsOriginBeginning, 76)!=errNone)
			return NULL;

		if ((VFSFileRead(dbo->fileRef, 2, &cnt, &read)!=errNone) ||  (read!=2))
			return NULL;

		if (VFSFileSeek(dbo->fileRef, vfsOriginBeginning, (((UInt32)recIdx)*8)+78)!=errNone)
			return NULL;

		if ((VFSFileRead(dbo->fileRef, 8, recEntry, &read)!=errNone) ||  (read!=8))
			return NULL;

		startOfRecord=*(UInt32 *)recEntry;
		if (recIdx==(cnt-1)) {
			if (VFSFileSize(dbo->fileRef, &endOfRecord)!=errNone)
				return NULL;
		} else {
			if (VFSFileSeek(dbo->fileRef, vfsOriginBeginning, ((((UInt32)recIdx+1))*8)+78)!=errNone)
				return NULL;

			if (VFSFileRead(dbo->fileRef, 8, recEntry, NULL)!=errNone)
				return NULL;

			endOfRecord=*(UInt32 *)recEntry;
		}

		if (VFSFileSeek(dbo->fileRef, vfsOriginBeginning, startOfRecord)!=errNone)
			return NULL;

		chunkSize=endOfRecord-startOfRecord;
		if ((mh=MemHandleNew(chunkSize))==NULL)
			return NULL;

		if ((VFSFileRead(dbo->fileRef, chunkSize, MemHandleLock(mh), &read)!=errNone) || (read!=chunkSize)) {
			MemHandleFree(mh);
			return NULL;
		}

		MemHandleUnlock(mh);
	}

	return mh;
}

/*
 * DBNumRecords
 *
 * Return number of records in database.
 *
 *  -> dbo		Database object.
 *
 * Returns number of records (or 0 if empty or error!)
 */
UInt16
DBNumRecords(DB *dbo)
{
	UInt32 read;
	UInt16 cnt;

	if (!dbo->path)
		return DmNumRecordsInCategory(dbo->db, dmAllCategories);

	if (VFSFileSeek(dbo->fileRef, vfsOriginBeginning, 76)!=errNone)
		return 0;

	if ((VFSFileRead(dbo->fileRef, 2, &cnt, &read)!=errNone) ||  (read!=2))
		return 0;

	return cnt;
}
