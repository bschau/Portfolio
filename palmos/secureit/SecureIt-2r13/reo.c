#include "sit.h"

/* macros */
#define OLDDATABASENAME "secureit-db"
#define OLDTITLELEN 32
#define OLDSECRETLEN 64
#define OLDCREATORID ((UInt32)'SeSt')
#define OLDDATABASETYPE ((UInt32)'cntn')

/* protos */
static Int16 reocompare(UInt8 *, UInt8 *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
static void internalreoto2r5(UInt8 *);

/*******************************************************************************
*
*	reo1to2
*
*	Reorganize data.  Copy from old database to new.
*	This updates a v1rX to v2r1.
*/
DmOpenRef
reo1to2(void)
{
	DmOpenRef olddb;
	LocalID dbid, appid;
	UInt16 idx, card, attrs, recidx, recsize;
	oldaiblock *aib;
	Int16 err;
	UInt8 oldtitle[OLDTITLELEN+2], oldsecret[OLDSECRETLEN+2];
	UInt8 *mp, *p;
	UInt8 catname[dmCategoryLength], sz;
	MemHandle mh, rec;

	dbid=DmFindDatabase(0, OLDDATABASENAME);
	if (!dbid)	/* nothing to do */
		return NULL;

	olddb=DmOpenDatabase(0, dbid, dmModeReadOnly);
	if (!olddb)	/* dammit! */
		return NULL;

	err=DmOpenDatabaseInfo(olddb, NULL, NULL, NULL, &card, NULL);
	if (err) 
		return olddb;

	appid=DmGetAppInfoID(olddb);
	aib=(oldaiblock *)MemLocalIDToLockedPtr(appid, card);
	if (!aib)
		return olddb;

	md5pwd[0]=aib->md5[0];
	md5pwd[1]=aib->md5[1];
	md5pwd[2]=aib->md5[2];
	md5pwd[3]=aib->md5[3];
	MemPtrUnlock(aib);
	
	err=setpwdindb(1);
	if (err)
		return olddb;

	/* nuke categories in new db - they will be copied from old db 
	   keep 'Unfiled' */
	for (idx=1; idx<dmRecNumCategories; idx++)
		CategorySetName(sitdb, idx, NULL);

	/* copy ... */
	oldtitle[OLDTITLELEN]='\x00';
	oldsecret[OLDSECRETLEN]='\x00';
	for (idx=0; idx<DmNumRecords(olddb); idx++) {
		mh=DmQueryRecord(olddb, idx);
		if (mh) {
			DmRecordInfo(olddb, idx, &attrs, NULL, NULL);
			attrs&=dmRecAttrCategoryMask;

			/* copy category */
			CategoryGetName(olddb, attrs, catname);
			CategorySetName(sitdb, attrs, catname);

			/* copy record */
			mp=MemHandleLock(mh);
			MemMove(oldtitle, mp, OLDTITLELEN);
			MemMove(oldsecret, mp+OLDTITLELEN, OLDSECRETLEN);
			MemHandleUnlock(mh);

			/* rebuild record */
			sz=StrLen(oldtitle);
			recsize=4+sz+OLDSECRETLEN;
			mh=MemHandleNew(recsize);
			if (mh) {
				mp=MemHandleLock(mh);
				p=mp;

				/* secret */
				*p++=0;
				*p++=OLDSECRETLEN;
				MemMove(p, oldsecret, OLDSECRETLEN);
				p+=OLDSECRETLEN;
				*p++='\x00';

				/* title */
				MemMove(p, oldtitle, sz);
				p+=sz;
				*p='\x00';

				recidx=dmMaxRecordIndex;
				rec=DmNewRecord(sitdb, &recidx, recsize);
				if (rec) {
					DmWrite(MemHandleLock(rec), 0, mp, recsize);
					MemHandleUnlock(rec);
					DmSetRecordInfo(sitdb, recidx, &attrs, NULL);
					DmReleaseRecord(sitdb, recidx, true);
					DmInsertionSort(sitdb, (DmComparF *)reocompare, 0);
				}
				MemHandleFree(mh);
			}
		}
	}

	nopwd=0;
	catlist.idx=0;
	return olddb;
}

/*******************************************************************************
*
*	reocompare
*
*	Comparison routine for reo.
*/
static Int16
reocompare(UInt8 *r1,
           UInt8 *r2,
           Int16 unused1,
           SortRecordInfoPtr unused2,
           SortRecordInfoPtr unused3,
           MemHandle unused4)
{
	return StrCompare(r1+2, r2+2);
}

/*******************************************************************************
*
*	reoto2r5
*
*	Sets new creatorid and type on secrets and templates databases.
*	This updates a v2rX to v2r5.
*/
void
reoto2r5(void)
{
	internalreoto2r5(SECRETSDATABASENAME);
	internalreoto2r5(TEMPLATESDATABASENAME);
}

/*******************************************************************************
*
*	internalreoto2r5
*/
static void
internalreoto2r5(UInt8 *dbn)
{
	UInt8 dbname[dmDBNameLength];
	Boolean err;
	UInt32 type, creator;
	LocalID dbid;

	MemSet(dbname, dmDBNameLength, 0);
	StrNCopy(dbname, dbn, dmDBNameLength-1);

	dbid=DmFindDatabase(0, dbname);
	if (!dbid)
		return;		/* no database .. */

	err=DmDatabaseInfo(0, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &type, &creator);
	if (err!=errNone)
		return;

	if (creator==OLDCREATORID || type==OLDDATABASETYPE) {
		creator=CREATORID;
		type=DATABASETYPE;
		DmSetDatabaseInfo(0, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &type, &creator);
	}
}
