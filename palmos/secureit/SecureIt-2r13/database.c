/*******************************************************************************
*
*	database.c
*
*	Database functions.
*/
#include "sit.h"

/* globals */
static UInt8 *untitledentry="Untitled";

/* protos */
static Int16 opensecretsdatabase(void);
static Int16 opentemplatesdatabase(void);
static UInt8 *keyvalidate(UInt8 *);

/*******************************************************************************
*
*	opendbs
*
*	Open databases.
*/
Int16
opendbs(void)
{
	Int16 err;

	err=opensecretsdatabase();
	if (!err) {
		err=opentemplatesdatabase();
		if (!err) {
			setdbprotect();
			return 0;
		}
	}

	return -1;
}

/*******************************************************************************
*
*	closedbs
*
*	Close databases.
*/
void
closedbs(void)
{
	if (tmpldb)
		DmCloseDatabase(tmpldb);

	if (sitdb)
		DmCloseDatabase(sitdb);
}

/*******************************************************************************
*
*	opensecretsdatabase
*
*	Opens the global secure it database.
*/
static Int16
opensecretsdatabase(void)
{
	UInt16 dbver=DATABASEVERSION, card;
	LocalID dbid, appid;
	Err err;
	aiblock *ai;
	MemHandle h;
	UInt8 dbname[dmDBNameLength];
	DmOpenRef olddb;

	MemSet(dbname, dmDBNameLength, 0);
	StrNCopy(dbname, SECRETSDATABASENAME, dmDBNameLength-1);

	dbid=DmFindDatabase(0, dbname);
	if (!dbid) {	/* create database */
		nopwd=1;
		err=DmCreateDatabase(0, dbname, CREATORID, DATABASETYPE, false);
		if (err)
			return -1;

		dbid=DmFindDatabase(0, dbname);
		sitdb=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!sitdb)
			return -1;

		/* create aiblock */
		err=DmOpenDatabaseInfo(sitdb, &dbid, NULL, NULL, &card, NULL);
		if (err)
			return -1;

		h=DmNewHandle(sitdb, sizeof(aiblock));
		if (!h)
			return -1;

		appid=MemHandleToLocalID(h);
		DmSetDatabaseInfo(card, dbid, NULL, NULL, &dbver, NULL, NULL, NULL, NULL, &appid, NULL, NULL, NULL);
		ai=(aiblock *)MemHandleLock(h);
		DmSet(ai, 0, sizeof(aiblock), 0);

		CategoryInitialize((AppInfoPtr)ai, sfmaincat);

		MemPtrUnlock(ai);

		olddb=reo1to2();
		if (olddb)
			DmCloseDatabase(olddb);
	} else {	/* existing database */
		nopwd=0;
		sitdb=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!sitdb)
			return -1;

		err=DmOpenDatabaseInfo(sitdb, &dbid, NULL, NULL, &card, NULL);
		if (err)
			return -1;

		err=DmDatabaseInfo(card, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appid, NULL, NULL, NULL);
		if (err)
			return -1;

		ai=(aiblock *)MemLocalIDToLockedPtr(appid, card);
		if (ai->pwdgiven)
			MemMove(md5pwd, ai->md5, sizeof(UInt32)*4);
		else
			nopwd=2;

		pwdgiven=ai->pwdgiven;
		MemPtrUnlock((MemPtr)ai);
	}

	return 0;
}

/*******************************************************************************
*
*	opentemplatesdatabase
*
*	Opens the templates database.
*/
static Int16
opentemplatesdatabase(void)
{
	UInt16 dbver=DATABASEVERSION, card;
	LocalID dbid;
	Err err;
	UInt8 dbname[dmDBNameLength];

	MemSet(dbname, dmDBNameLength, 0);
	StrNCopy(dbname, TEMPLATESDATABASENAME, dmDBNameLength-1);

	dbid=DmFindDatabase(0, dbname);
	if (!dbid) {	/* create database */
		err=DmCreateDatabase(0, dbname, CREATORID, DATABASETYPE, false);
		if (err)
			return -1;

		dbid=DmFindDatabase(0, dbname);
		tmpldb=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!tmpldb)
			return -1;

		err=DmOpenDatabaseInfo(tmpldb, &dbid, NULL, NULL, &card, NULL);
		if (err)
			return -1;

		DmSetDatabaseInfo(card, dbid, NULL, NULL, &dbver, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

		preloadtmpl=1;
	} else {	/* existing database */
		tmpldb=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!tmpldb)
			return -1;

		preloadtmpl=0;
	}

	return 0;
}

/*******************************************************************************
*
*	storerecord
*
*	Stores (insert/update) a record in database.
*/
Boolean
storerecord(DmOpenRef db,
             UInt8 *key,
             UInt8 *txt,
             UInt16 cat,
             UInt16 idx)
{
	UInt16 attr;
	MemHandle rec, dr;
	UInt32 recsize;

	rec=formrecord(true, &recsize, key, txt);
	if (!rec)
		return false;

	if (idx==dmMaxRecordIndex) {	/* seach for a position */
		idx=DmFindSortPosition(db, MemHandleLock(rec), NULL, (DmComparF *)recordcompare, 0);
		MemHandleUnlock(rec);

		dr=DmNewRecord(db, &idx, recsize);
	} else
		dr=DmResizeRecord(db, idx, recsize);

	if (dr) {
		DmWrite(MemHandleLock(dr), 0, MemHandleLock(rec), recsize);
		MemHandleUnlock(dr);
		MemHandleFree(rec);

		attr=cat|dmRecAttrDirty;
		DmSetRecordInfo(db, idx, &attr, NULL);
		DmReleaseRecord(db, idx, true);

		DmInsertionSort(db, (DmComparF *)recordcompare, 0);
		return true;
	}

	MemHandleFree(rec);
	return false;
}

/*******************************************************************************
*
*	recorddesc
*
*	Set record description.
*/
void
recorddesc(recdesc *rd,
           UInt8 *rec)
{
	UInt32 len;

	len=GETOFFSET(rec);
	rd->secretlen=len;
	rd->secret=rec+2;
	
	rec+=3+len;
	rd->title=rec;
	rd->titlelen=StrLen(rec);
}

/*******************************************************************************
*
*	keyvalidate
*
*	Validates key.  That is ... strips leading spaces etc. etc.
*/
static UInt8 *
keyvalidate(UInt8 *key)
{
	if (key) {
		while (*key && (*key==' ' || *key=='\n' || *key=='\r' || *key=='\t'))
			key++;

		if (*key)
			return key;
	}

	return untitledentry;
}

/*******************************************************************************
*
*	formrecord
*
*	Creates a record.
*/
MemHandle
formrecord(Boolean untitle,
           UInt32 *size,
           UInt8 *key,
           UInt8 *txt)
{
	UInt32 recsize=4, keylen, txtlen, cnt, alignedsize;
	MemHandle rec;
	UInt8 *recp;
	blowfishkey *pk;

	if (untitle==true)
		key=keyvalidate(key);

	keylen=StrLen(key);
	txtlen=StrLen(txt);

	alignedsize=((txtlen+7)/8)*8;

	recsize+=(alignedsize+keylen);
	*size=recsize;
	rec=MemHandleNew(recsize);
	if (!rec)
		return NULL;	/* bummer */

	recp=MemHandleLock(rec);
	*recp++=((alignedsize>>8)&0x00ff);
	*recp++=(alignedsize&0x00ff);
	if (txtlen>0) {
		MemMove(recp, txt, txtlen);
		MemSet(recp+txtlen, alignedsize-txtlen, 0);

		pk=MemHandleLock(mainkeyh);
		for (cnt=0; cnt<(alignedsize/8); cnt++) {
			blowfishencrypt(pk, recp);
			recp+=8;
		}
		MemHandleUnlock(mainkeyh);
	}
	*recp++='\x00';

	MemMove(recp, key, keylen);
	recp[keylen]='\x00';

	MemHandleUnlock(rec);
	return rec;
}

/*******************************************************************************
*
*	recordcompare
*
*	Compare function for sort functions.
*
*	See reference manual for arguments.
*/
Int16
recordcompare(UInt8 *r1,
              UInt8 *r2,
              Int16 unused1,
              SortRecordInfoPtr unused2,
              SortRecordInfoPtr unused3,
              MemHandle unused4)
{
	UInt32 o1, o2;

	o1=GETOFFSET(r1)+3;
	r1+=o1;

	o2=GETOFFSET(r2)+3;
	r2+=o2;

	return StrCaselessCompare(r1, r2);
}
