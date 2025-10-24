/*
**	db.c
**
**	Database functions.
*/
#include "eventor.h"

/* structs */
enum { FDFound, FDNotFound, FDError };

/* protos */
static UInt16 FindDatabase(DB *);
static void Panic(DB *);

/*
**	EventorCompareFunction
*/
Int16
EventorCompareFunction(void *r1, void *r2, Int16 u1, SortRecordInfoPtr u2, SortRecordInfoPtr u3, MemHandle u4)
{
	TEvent *e1=r1, *e2=r2;
	UInt16 d1, d2;

	/* compare dates */
	d1=(e1->date)&MaskOnlyDate;
	d2=(e2->date)&MaskOnlyDate;
	if (d1<d2)
		return -1;

	if (d1>d2)
		return 1;

	/* then event names */
	return TxtGlueCaselessCompare(e1->event, StrLen(e1->event), &d1, e2->event, StrLen(e2->event), &d2);
}

/*
**	FindDatabase
**
**	Search all cards for a given database.
**
**	 -> dbo		Database object.
**
**	Returns FDFound, FDNotFound or FDError.
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
**	Panic
**
**	Delete database & reset device.
**
**	 -> dbo		Database object.
*/
static void
Panic(DB *dbo)
{
	DmDeleteDatabase(dbo->card, dbo->id);
	ErrFatalDisplay("(Panic) Major database error - resetting.");
	SysReset();
}

/*
**	DBCreateDatabases
**
**	Create all databases.
*/
void
DBCreateDatabases(void)
{
	UInt16 dbVer=DBVERSION, idx;
	DB dbo, oEvDb;
	MemHandle h;
	LocalID appID;
	AppInfoBlock *appInfo;

	DBInit(&dbo, DBNAME);
	if (FindDatabase(&dbo)!=FDNotFound)
		return;

	ErrFatalDisplayIf(DmCreateDatabase(0, dbo.name, CRID, DBTYPE, false)!=errNone, "(DBCreateDatabase) Cannot create database.");
	if (FindDatabase(&dbo)!=FDFound)
		Panic(&dbo);

	if (DBOpen(&dbo, dmModeReadWrite, "Data")==false)
		Panic(&dbo);

	if ((h=DmNewHandle(dbo.db, sizeof(AppInfoBlock)))==NULL)
		Panic(&dbo);

	appID=MemHandleToLocalID(h);
	DmSetDatabaseInfo(dbo.card, dbo.id, NULL, NULL, &dbVer, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL);
	appInfo=(AppInfoBlock *)MemHandleLock(h);
	DmSet(appInfo, 0, sizeof(AppInfoBlock), 0);
	CategoryInitialize((AppInfoPtr)appInfo, defaultCategories);
	MemPtrUnlock(appInfo);

	DBInit(&oEvDb, OLDDBNAME);
	if ((idx=FindDatabase(&oEvDb))==FDError)
		Panic(&dbo);

	if (idx==FDFound) {
		if (DBOpen(&oEvDb, dmModeReadOnly, "Pre v2r1 Data")==false)
			Panic(&dbo);

		if (Reo1To2(&dbo, &oEvDb)==false)
			Panic(&dbo);

		DBClose(&oEvDb);
		DmDeleteDatabase(oEvDb.card, oEvDb.id);
	}

	DBClose(&dbo);
}
		
/*
**	DBInit
**
**	Initialize a database structure.
**
**	 -> dbo		Database object.
**	 -> name	Database name.
*/
void
DBInit(DB *dbo, Char *name)
{
	MemSet(dbo, sizeof(DB), 0);
	StrNCopy(dbo->name, name, dmDBNameLength);
}

/*
**	DBOpen
**
**	Open a database. Issue warning if unsuccessful.
**
**	 -> dbo		Database object.
**	 -> mode	Open mode.
**	 -> txt		Text name of database (NULL = no alert).
**
**	Returns true if database opened successfully, false otherwise.
*/
Boolean
DBOpen(DB *dbo, UInt16 mode, Char *txt)
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

	if (txt)
		FrmCustomAlert(aCannotOpenDatabase, txt, NULL, NULL);

	return false;
}

/*
**	DBClose
**
**	Close a database.
**
**	 -> dbo		Database object.
*/
void
DBClose(DB *dbo)
{
	if (dbo->db)
		DmCloseDatabase(dbo->db);

	MemSet(dbo, sizeof(DB), 0);
}

/*
**	DBSetRecord
**
**	Set (or adds a record) to the database.
**
**	 -> dbo		Database object.
**	 -> catIdx	Category index.
**	 -> rec		Record.
**	 -> recLen	Record length.
**	 -> rIdx	Record index (dmMaxRecordIndex to add).
*/
void
DBSetRecord(DB *dbo, UInt16 catIdx, void *rec, UInt16 recLen, UInt16 rIdx)
{
	DmOpenRef db=dbo->db;
	UInt16 attr=catIdx|dmRecAttrDirty;
	MemHandle dr;

	if (rIdx==dmMaxRecordIndex)
		dr=DmNewRecord(db, &rIdx, recLen);
	else
		dr=DmGetRecord(db, rIdx);

	ErrFatalDisplayIf(dr==NULL, "(DBSetRecord) Cannot allocate new record or overwrite old.");

	DmWrite(MemHandleLock(dr), 0, rec, recLen);
	MemHandleUnlock(dr);

	DmSetRecordInfo(db, rIdx, &attr, NULL);
	DmReleaseRecord(db, rIdx, true);

	DmInsertionSort(db, (DmComparF *)EventorCompareFunction, 0);
}

/*
**	DBGetRecord
**
**	Retrieves a record at a given index in a given database.
**
**	 -> dbo		Database object.
**	 -> recIdx	Record index.
**
**	Returns MemHandle.
*/
MemHandle
DBGetRecord(DB *dbo, UInt16 recIdx)
{
	MemHandle mh;

	mh=DmQueryRecord(dbo->db, recIdx);
	ErrFatalDisplayIf(mh==NULL, "(DBGetRecord) Cannot retrieve record.");
	
	return mh;
}
