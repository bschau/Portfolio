#include "loginman.h"

/* protos */
static Int16 dbcompare(UInt8 *, UInt8 *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
static void obfuscate(UInt8 *, UInt16);
static void defuscate(UInt8 *, UInt16);

/*******************************************************************************
*
*	opendatabase
*
*	Open or create database.
*/
DmOpenRef
opendatabase(void)
{
	DmOpenRef db=NULL;
	UInt16 dbver=DATABASEVERSION, card, attr;
	LocalID dbid;
	Err err;
	UInt8 dbname[dmDBNameLength];

	MemSet(dbname, dmDBNameLength, 0);
	StrNCopy(dbname, QALMDATABASENAME, dmDBNameLength-1);

	dbid=DmFindDatabase(0, dbname);
	if (!dbid) {	/* create database */
		err=DmCreateDatabase(0, dbname, CREATORID, DATABASETYPE, false);
		if (err)
			return NULL;

		dbid=DmFindDatabase(0, dbname);
		if (!dbid)
			return NULL;

		db=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!db)
			return NULL;

		/* make it hotsyncable */
		err=DmOpenDatabaseInfo(db, &dbid, NULL, NULL, &card, NULL);
		if (err==errNone) {
			err=DmDatabaseInfo(card, dbid, NULL, &attr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			if (err==errNone) {
				attr|=dmHdrAttrBackup;
				attr&=~dmHdrAttrCopyPrevention;

				err=DmSetDatabaseInfo(card, dbid, NULL, &attr, &dbver, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			 }
		}

		if (err!=errNone)
			FrmCustomAlert(alerthotsync, NULL, NULL, NULL);

		DEBUGCOMMAND(preload(db));

	} else {	/* open database */
		db=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!db)
			return NULL;
	}

	return db;
}

/*******************************************************************************
*
*	closedatabase
*
*	Close database (!)
*/
void
closedatabase(DmOpenRef db)
{
	if (db)
		DmCloseDatabase(db);
}

/*******************************************************************************
*
*	qpack
*
*	Pack realm, login and password into one structure.
*/
void
qpack(realmpacked *rp,
      UInt8 *realm,
      UInt8 *login,
      UInt8 *password)
{
	UInt16 rlen, llen, plen, packedlen, rllen, rplen;
	MemHandle mh;
	UInt8 *p;

	rlen=StrLen(realm);

	if (login)
		llen=StrLen(login);
	else
		llen=0;

	if (password)
		plen=StrLen(password);
	else
		plen=0;

	rllen=(llen+1)>>1;
	rllen<<=1;
	rplen=(plen+1)>>1;
	rplen<<=1;
	packedlen=6+rlen+1+rllen+1+rplen+1;

	mh=MemHandleNew(packedlen);
	ErrFatalDisplayIf(mh==NULL, "(qpack) Out of memory");

	p=MemHandleLock(mh);
	MemSet(p, packedlen, 0);

	*p++=(rlen>>8)&0xff;
	*p++=rlen&0xff;
	*p++=(rllen>>8)&0xff;
	*p++=rllen&0xff;
	*p++=(rplen>>8)&0xff;
	*p++=rplen&0xff;

	MemMove(p, realm, rlen);
	p+=rlen+1;

	if (rllen) {
		MemMove(p, login, llen);
		obfuscate(p, rllen);
		*(p+rllen)='\x00';
		p+=rllen+1;
	}

	if (rplen) {
		MemMove(p, password, plen);
		obfuscate(p, rplen);
		*(p+rplen)='\x00';
	}

	MemHandleUnlock(mh);

	rp->size=packedlen;
	rp->mh=mh;
}

/*******************************************************************************
*
*	qunpack
*
*	Unpack a packed realm record to a realmrecord.
*/
void
qunpack(realmrecord *rr,
        UInt8 *bs)
{
	UInt32 rlen, llen, plen;

	rlen=GETOFFSET(bs);
	bs+=2;

	llen=GETOFFSET(bs);
	bs+=2;

	plen=GETOFFSET(bs);
	bs+=2;

	rr->realm=bs;

	bs+=rlen+1;

	if (llen) {
		rr->login=bs;
		defuscate(bs, llen);

		bs+=llen+1;
	} else
		rr->login=NULL;

	if (plen) {
		rr->password=bs;
		defuscate(bs, plen);
	} else
		rr->password=NULL;
}

/*******************************************************************************
*
*	obfuscate
*
*	Obfuscate data ... - this is insecure ...
*/
static void
obfuscate(UInt8 *data,
          UInt16 dlen)
{
	UInt16 val[16]={ 1, 3, 5, 7, 6, 4, 2, 0, 15, 13, 11, 9, 8, 10, 12, 14 };
	UInt16 adj=1, i;
	UInt8 b1, b2, b3, b4;

	for (i=0; i<(dlen/2); i++) {
		b1=(*data)+adj;
		b2=(*(data+1))+adj+1;
		adj+=2;

		b3=val[(b1>>4)&0xf];	/* hi 1 */
		b3|=(val[b2&0xf])<<4;	/* low 2 */

		b4=(val[b1&0xf])<<4;	/* low 1 */
		b4|=val[(b2>>4)&0xf];	/* hi 2 */

		*data=b3;
		*(data+1)=b4;
		data+=2;
	}
}

/*******************************************************************************
*
*	defuscate
*
*	Undo obfuscation.
*/
static void
defuscate(UInt8 *data,
          UInt16 dlen)
{
	UInt16 val[16]={ 7, 0, 6, 1, 5, 2, 4, 3, 12, 11, 13, 10, 14, 9, 15, 8 };
	UInt16 adj=1, i;
	UInt8 b1, b2, b3, b4;

	for (i=0; i<(dlen/2); i++) {
		b1=*data;
		b2=*(data+1);

		b3=(val[b1&0xf])<<4;
		b3|=val[(b2>>4)&0xf];

		b4=(val[b2&0xf])<<4;
		b4|=val[(b1>>4)&0xf];

		*data=(b3-adj);
		*(data+1)=(b4-adj-1);
		adj+=2;
		data+=2;
	}
}

/*******************************************************************************
*
*	storerecord
*
*	Store a record.
*/
void
storerecord(DmOpenRef db,
            realmpacked *rp,
            UInt16 idx)
{
	UInt16 sort=false;
	MemHandle rec;
	
	if (idx==dmMaxRecordIndex) {
		sort=true;
		idx=0;
		rec=DmNewRecord(db, &idx, rp->size);
	} else
		rec=DmResizeRecord(db, idx, rp->size);

	ErrFatalDisplayIf(rec==NULL, "(storerecord) Out of memory");

	DmWrite(MemHandleLock(rec), 0, MemHandleLock(rp->mh), rp->size);
	MemHandleUnlock(rp->mh);
	MemHandleUnlock(rec);

	DmReleaseRecord(db, idx, true);

	if (sort==true)
		DmInsertionSort(db, (DmComparF *)dbcompare, 0);
}

/*******************************************************************************
*
*	dbcompare
*
*	Sort function for DmInsertionSort.
*/
static Int16
dbcompare(UInt8 *r1,
          UInt8 *r2,
          Int16 unused1,
          SortRecordInfoPtr unused2,
          SortRecordInfoPtr unused3,
          MemHandle unused4)
{
	return StrCaselessCompare(r1+6, r2+6);
}

/*******************************************************************************
*
*	findrecord
*
*	Return record index - dmMaxRecordIndex if record is not found.
*/
UInt16
findrecord(DmOpenRef db,
           MemHandle rec)
{
	UInt16 idx;
	Int16 c;
	MemHandle mh;

	idx=DmFindSortPosition(db, MemHandleLock(rec), NULL, (DmComparF *)dbcompare, 0);
	MemHandleUnlock(rec);

	if (idx) {
		idx--;
		mh=DmQueryRecord(db, idx);
		if (mh) {
			c=StrCaselessCompare(MemHandleLock(rec)+6, MemHandleLock(mh)+6);

			MemHandleUnlock(mh);
			MemHandleUnlock(rec);

			if (!c)
				return idx;
		}
	}
	return dmMaxRecordIndex;
}

/*******************************************************************************
*
*	clonerecord
*
*	Clones a record handle.
*/
MemHandle
clonerecord(DmOpenRef db,
            UInt16 recidx)
{
	UInt16 size, s;
	MemHandle mh, ch;
	UInt8 *mp, *p;

	mh=DmQueryRecord(db, recidx);
	if (!mh)
		return NULL;

	mp=MemHandleLock(mh);
	p=mp;
	size=9;	/* 3 * (length + term zero) */
	
	s=GETOFFSET(p);
	size+=s;
	p+=2;
	s=GETOFFSET(p);
	size+=s;
	p+=2;
	s=GETOFFSET(p);
	size+=s;

	ch=MemHandleNew(size);
	ErrFatalDisplayIf(ch==NULL, "(clonerecord) Out of memory");

	MemMove(MemHandleLock(ch), mp, size);
	MemHandleUnlock(ch);
	MemHandleUnlock(mh);

	return ch;
}

/*******************************************************************************
*
*	fakerecord
*
*	Create a fake record with the given key.
*/
MemHandle
fakerecord(UInt8 *key)
{
	UInt16 len=StrLen(key);
	MemHandle mh;
	UInt8 *mp;

	mh=MemHandleNew(len+6+1);
	ErrFatalDisplayIf(mh==NULL, "(fakerecord) Out of memory");

	mp=MemHandleLock(mh);
	MemSet(mp, len+6+1, 0);

	MemMove(mp+6, key, len);

	*mp++=(len>>8)&0xff;
	*mp=len&0xff;
	MemHandleUnlock(mh);
	return mh;
}
