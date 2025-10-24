/*
**	db.c
**
**	Database functions.
*/
#include "bmilog.h"

/* structs */
enum { FDFound, FDNotFound, FDError };

/* protos */
static UInt16 FindDatabase(DB *);

/*
**	DBCompare
**
**	See reference guide for arguments.
*/
Int16
DBCompare(void *r1, void *r2, Int16 u1, SortRecordInfoPtr u2, SortRecordInfoPtr u3, MemHandle u4)
{
	Record *p1=r1, *p2=r2;
	Char *n1, *n2;
	UInt16 r1m, r2m;
	Person *o1, *o2;
	BMI *b1, *b2;

	if (p1->type<p2->type)
		return -1;
	else if (p1->type>p2->type)
		return 1;

	switch (p1->type) {
	case RPerson:
		o1=(Person *)p1;
		o2=(Person *)p2;
		if (o1->r.id<o2->r.id)
			return -1;
		else if (o1->r.id>o2->r.id)
			return 1;

		n1=o1->name;
		n2=o2->name;
		return TxtGlueCaselessCompare(n1, StrLen(n1), &r1m, n2, StrLen(n2), &r2m);
	case RBmi:
		b1=(BMI *)p1;
		b2=(BMI *)p2;
		if (b1->r.id<b2->r.id)
			return -1;
		else if (b1->r.id>b2->r.id)
			return 1;

		r1m=b1->date;
		r2m=b2->date;
		if (r2m<r1m)
			return -1;
		if (r2m>r1m)
			return 1;
		return 0;
	}

	ErrFatalDisplay("(DBCompare) Database inconsistancy - I cowardly refuses to go on.");
	return 0;
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

		switch (DmGetLastErr()) {
		case dmErrCantFind:
			break;
		default:
			return FDError;
		}
	}

	return FDNotFound;
}

/*
**	DBCreateDatabase
**
**	Create the database.
*/
void
DBCreateDatabase(void)
{
	UInt16 dbVer=DBVERSION;
	DB dbo;

	DBInit(&dbo, DBNAME);
	switch (FindDatabase(&dbo)) {
	case FDNotFound:
		ErrFatalDisplayIf(DmCreateDatabase(0, dbo.name, CRID, 'DATA', false)!=errNone, "(DBCreateDatabase) Cannot create database.");
		if (FindDatabase(&dbo)==FDFound)
			DmSetDatabaseInfo(dbo.card, dbo.id, NULL, NULL, &dbVer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	default:	/* FALL-THRU */
		break;
	}
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
	StrNCopy(dbo->name, name, dmDBNameLength-1);
}

/*
**	DBOpen
**
**	Open a database.
**
**	 -> dbo		Database object.
**	 -> mode	Open mode.
**
**	Returns true if database opened successfully, false otherwise.
*/
Boolean
DBOpen(DB *dbo, UInt16 mode)
{
	if (FindDatabase(dbo)==FDFound) {
		if ((dbo->db=DmOpenDatabase(dbo->card, dbo->id, mode))!=NULL)
			return true;
	}

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
**	DBOpenDatabase
**
**	Open a database. Issue warning if unsuccessful.
**
**	 -> dbo		Database object.
**	 -> mode	Open mode.
**	 -> txt		Text name of database (NULL = no alert).
**
**	Returns true if successful, false otherwise.
*/
Boolean
DBOpenDatabase(DB *dbo, UInt16 mode, Char *txt)
{
	if (dbo->db)
		return true;	/* Already open */

	if (DBOpen(dbo, mode)==true)
		return true;

	if (txt)
		FrmCustomAlert(aCannotOpenDatabase, txt, NULL, NULL);

	return false;
}

/*
**	DBSetRecord
**
**	Set (or adds a record) to the database.
**
**	 -> dbo		Database object.
**	 -> rec		Record.
**	 -> rIdx	Record index (dmMaxRecordIndex to add).
**
**	This function destroys the input record!
*/
void
DBSetRecord(DB *dbo, MemHandle rec, UInt16 rIdx)
{
	DmOpenRef db=dbo->db;
	UInt16 attr=dmRecAttrDirty;
	UInt32 recLen;
	MemHandle dr;

	recLen=MemHandleSize(rec);

	if (rIdx==dmMaxRecordIndex)
		dr=DmNewRecord(db, &rIdx, recLen);
	else
		dr=DmResizeRecord(db, rIdx, recLen);

	ErrFatalDisplayIf(dr==NULL, "(DBSetRecord) Cannot allocate new record or resize old.");

	DmWrite(MemHandleLock(dr), 0, MemHandleLock(rec), recLen);
	MemHandleUnlock(dr);
	MemHandleFree(rec);

	DmSetRecordInfo(db, rIdx, &attr, NULL);
	DmReleaseRecord(db, rIdx, true);

	DmInsertionSort(db, (DmComparF *)DBCompare, 0);
}

/*
**	DBDeleteRecord
**
**	Delete a record possible with confirmation.
**
**	 -> dbo		Database object.
**	 -> recIdx	Record Index.
**	 -> id		Alert ID (don't show confirmation box if 0).
**	 -> text	Extra text for dialog.
**
**	Returns true if record was deleted, false otherwise.
*/
Boolean
DBDeleteRecord(DB *dbo, UInt16 recIdx, UInt16 id, Char *text)
{
	if (id) {
		if (FrmCustomAlert(id, text, NULL, NULL)==1)
			return false;
	}

	DmDeleteRecord(dbo->db, recIdx);
	return true;
}

/*
**	DBTypeIdSearch
**
**	Search a database for a given object type and id.
**
**	 -> dbo		Database object.
**	 -> type	Object type.
**	 -> id		ID.
**	 -> eIdx	Existing index.
**
**	Returns the index if found, dmMaxRecordIndex otherwise.
*/
UInt16
DBTypeIdSearch(DB *dbo, UInt8 type, UInt32 id, UInt16 eIdx)
{
	DmOpenRef db=dbo->db;
	UInt16 recIdx=0;
	MemHandle mh;
	Record *p;

	for (EVER) {
		if (recIdx!=eIdx) {	// Let's not find ourself!
			if ((mh=DmQueryNextInCategory(db, &recIdx, dmAllCategories))==NULL)
				break;
	
			p=MemHandleLock(mh);
			if (p->type>type) {
				MemHandleUnlock(mh);
				break;
			}

			if (p->type==type) {
				if (p->id==id) {
					MemHandleUnlock(mh);
					return recIdx;
				}
			}

			MemHandleUnlock(mh);
		}
		recIdx++;
	}

	return dmMaxRecordIndex;
}
