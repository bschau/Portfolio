/*
 * DB.c
 */
#include "Include.h"

/* extern */
extern UInt16 libRefNum;
extern MemHandle aesData;

/*
 * DBSort
 *
 * Sort function.
 */
Int16
DBSort(void *r1, void *r2, Int16 u1, SortRecordInfoType *u2, SortRecordInfoType *u3, MemHandle u4)
{
	Char *emptyString="";
	RecordHdr *h1=(RecordHdr *)r1, *h2=(RecordHdr *)r2;
	Char *p1, *p2;

	if (h1->flags&RHHasTitle)
		p1=((Char *)h1)+sizeof(RecordHdr);
	else
		p1=emptyString;

	if (h2->flags&RHHasTitle)
		p2=((Char *)h2)+sizeof(RecordHdr);
	else
		p2=emptyString;

	return StrCaselessCompare(p1, p2);
}

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
	DBClear(dbo);
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
 *  -> flags		Flags.
 *  -> iconID		Icon ID.
 *  -> title		Title.
 *  -> secret		Secret.
 *  -> sort		Sort function.
 *
 * Returns record unique id.
 */
UInt32
DBSetRecord(DB *dbo, UInt16 rIdx, UInt16 category, UInt16 flags, DmResID iconID, Char *title, Char *secret, DmComparF sort)
{
	DmOpenRef db=dbo->db;
	UInt32 total=sizeof(RecordHdr), idx=0, cnt, c;
	UInt16 tLen=0, sLen=0;
	Char nul=0;
	MemHandle rh;
	UInt8 buffer[32];
	UInt8 *p;
	UInt32 uid;
	UInt16 attrs;

	if (title)
		tLen=StrLen(title)+1;

	if (secret)
		sLen=StrLen(secret)+1;

	if (tLen) {
		flags|=RHHasTitle;

		total+=tLen;
		if (tLen&1)
			total++;
	}

	if (sLen)
		flags|=RHHasSecret;

	cnt=sLen/32;
	if (sLen%32)
		cnt++;

	total+=(cnt*32);

	if (rIdx==dmMaxRecordIndex)
		rh=DmNewRecord(db, &rIdx, total);
	else
		rh=DmResizeRecord(db, rIdx, total);

	ErrFatalDisplayIf(rh==NULL, "(DBSetRecord) Device is full.");

	p=MemHandleLock(rh);

	DmWrite(p, idx, &flags, sizeof(UInt16));
	idx+=sizeof(UInt16);
	DmWrite(p, idx, &iconID, sizeof(DmResID));
	idx+=sizeof(DmResID);

	if (tLen>0) {
		DmWrite(p, idx, title, tLen);
		idx+=tLen;

		if (tLen&1) {
			DmWrite(p, idx, &nul, 1);
			idx++;
		}
	}

	if (sLen>0) {
		for (c=0; c<cnt; c++) {
			if (sLen>=32)
				MemMove(buffer, secret, 32);
			else {
				MemSet(buffer, sizeof(buffer), 0);
				MemMove(buffer, secret, sLen);
			}

			AESEncrypt(buffer, buffer);
			AESEncrypt(buffer+16, buffer+16);
			DmWrite(p, idx, buffer, sizeof(buffer));

			sLen-=32;
			secret+=32;
			idx+=32;
		}
	}

	MemHandleUnlock(rh);

	DmRecordInfo(db, rIdx, &attrs, &uid, NULL);
	attrs&=~dmRecAttrCategoryMask;
	if (category<dmRecNumCategories)
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

/*
 * DBFindRecordByID
 *
 * Find a record by ID. This is a replacement of the DmFindRecordByID which
 * will barf if you give it a non-existing id.
 *
 *  -> dbo	Database object.
 *  -> id	Unique ID.
 * <-  rIdx	Return index.
 *
 * Returns errNone if record was found, dmErrUniqueIDNotFound otherwise.
 */
Err
DBFindRecordByID(DB *dbo, UInt32 id, UInt16 *rIdx)
{
	DmOpenRef db=dbo->db;
	UInt16 cnt;
	
	if ((cnt=DmNumRecords(db))>0) {
		UInt16 attrs=0, idx;
		UInt32 uid;

		*rIdx=0;
		for (idx=0; idx<cnt; idx++) {
			if (DmRecordInfo(db, idx, &attrs, &uid, NULL)==errNone) {
				if (!(attrs&dmRecAttrDelete)) {
					if (uid==id) {
						*rIdx=idx;
						return errNone;
					}
				}
			}
		}
	}

	*rIdx=dmMaxRecordIndex;
	return dmErrUniqueIDNotFound;
}
