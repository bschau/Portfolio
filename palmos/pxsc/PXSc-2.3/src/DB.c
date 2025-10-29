/*
 * db.c
 */
#include "Include.h"

/* structs */
enum { FDFound, FDNotFound, FDError };

/* protos */
static Int16 NameCompare(void *, void *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
static UInt16 FindDatabase(DB *);
static void Panic(DB *);

/*
 * NameCompare
 */
static Int16
NameCompare(void *r1, void *r2, Int16 u1, SortRecordInfoPtr u2, SortRecordInfoPtr u3, MemHandle u4)
{
	Char *e1=r1, *e2=r2;
	UInt16 d1, d2;

	return TxtGlueCaselessCompare(e1, StrLen(e1), &d1, e2, StrLen(e2), &d2);
}

/*
 * FindDatabase
 *
 * Search all cards for a given database.
 *
 *   -> dbo		Database object.
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
 *   -> dbo		Database object.
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

	DBInit(&dbo, DBNAME);
	if (FindDatabase(&dbo)!=FDNotFound)
		return;

	ErrFatalDisplayIf(DmCreateDatabase(0, dbo.name, CRID, DBTYPE, false)!=errNone, "(DBCreateDatabase) Cannot create database.");
	if (FindDatabase(&dbo)!=FDFound)
		Panic(&dbo);

	DmSetDatabaseInfo(dbo.card, dbo.id, NULL, &attrs, &dbVer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
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
	if (dbo->db)
		return true;	/* Already open */

	if (FindDatabase(dbo)==FDFound) {
		if ((dbo->db=DmOpenDatabase(dbo->card, dbo->id, mode))!=NULL)
			return true;
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
 * DBAddName
 *
 * Add a name to the names database.
 *
 *  -> rIdx		Record index (or dmMaxRecordIndex).
 *  -> name		Name of person.
 */
void
DBAddName(UInt16 rIdx, Char *name)
{
	UInt32 nLen=StrLen(name), idx;
	Boolean forceUpper=true;
	Char nameBuffer[PlayerNameLength+1];
	MemHandle rh;
	UInt16 attrs;
	Char *n;

	n=nameBuffer;
	for (idx=0; idx<nLen; idx++) {
		if (TxtGlueCharIsSpace(*name))
			forceUpper=true;
		else {
			if (forceUpper==true) {
				forceUpper=false;
				*n=TxtGlueUpperChar(*name);
			} else
				*n=TxtGlueLowerChar(*name);

			n++;
		}

		name++;
	}
	*n='\x00';

	if ((nLen=StrLen(nameBuffer)+1)==1)
		return;

	if (rIdx==dmMaxRecordIndex)
		rh=DmNewRecord(dbData.db, &rIdx, nLen);
	else
		rh=DmResizeRecord(dbData.db, rIdx, nLen);

	ErrFatalDisplayIf(rh==NULL, "(DBAddName) Device is full.");

	n=MemHandleLock(rh);
	DmWrite(n, 0, nameBuffer, nLen);
	MemHandleUnlock(rh);

	DmRecordInfo(dbData.db, rIdx, &attrs, NULL, NULL);
	attrs|=dmRecAttrDirty;
	DmSetRecordInfo(dbData.db, rIdx, &attrs, NULL);
	
	DmInsertionSort(dbData.db, NameCompare, 0);
}

/*
 * DBGetName
 *
 * Retrieves a record at a given index.
 *
 *  -> recIdx	Record index.
 *
 * Returns MemHandle.
 */
MemHandle
DBGetName(UInt16 recIdx)
{
	MemHandle mh;

	mh=DmQueryRecord(dbData.db, recIdx);
	ErrFatalDisplayIf(mh==NULL, "(DBGetName) Cannot retrieve name.");
	
	return mh;
}
