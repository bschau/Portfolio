/*
 * PhoneSearch.c
 */
#include "Include.h"

/* protos */
static Boolean Search(DB *, MemHandle *, UInt16 *);
static UInt16 FindInRecord(UInt8 *, UInt32);
static MemHandle GetPersonData(UInt8 *);

/* globals */
static Char forwardPhone[MaxPhoneLength], reversePhone[MaxPhoneLength];
static UInt16 filter, reversePhoneLen;

/*
 * PhoneSearch
 *
 * Search for the given phone number.
 *
 *  -> phn	Phone number (as entered by user).
 * <-  dMH	Destination memory handle.
 * <-  dOffset	Destination offset.
 *
 * Returns true if phone was found, false otherwise.
 */
Boolean
PhoneSearch(Char *phn, MemHandle *dMH, UInt16 *dOffset)
{
	UInt16 pLen=StrLen(phn), idx;
	Boolean found=false;
	Char *p=forwardPhone, *r=reversePhone;
	Char dbKey;
	UInt8 c;
	Char dbName[dmDBNameLength];
	DB db;

	for (idx=0; idx<pLen; idx++) {
		if (phn[idx]>='0' && phn[idx]<='9')
			*p++=phn[idx];
		else if (phn[idx]!='+' && phn[idx]!='.' && phn[idx]!=' ')
			break;
	}

	*p=0;
	while (p>forwardPhone)
		*r++=*--p;

	*r=0;
	r=reversePhone;
	dbKey=*r++;
	filter=((*r++)<<8)&0xff00;
	filter|=*r++;

	pLen=StrLen(r);
	p=reversePhone;
	for (idx=0; idx<((pLen>>1)<<1); ) {
		c=(((r[idx++])-'0')<<4)&0xf0;
		c|=((r[idx++])-'0')&0x0f;
		*p++=c;
	}

	if (idx<pLen) {
		c=(((r[idx++])-'0')<<4)&0xf0;
		*p++=c;
	}

	*p=0;
	reversePhoneLen=StrLen(reversePhone);

	StrPrintF(dbName, "PFOC_pfoc phones%c", dbKey);

	DBInit(&db, dbName, rootPath);
	if (DBFindDatabase(&db)==FDFound) {
		if (DBOpen(&db, dmModeReadOnly, true)==true) { 
			found=Search(&db, dMH, dOffset);
			DBClose(&db);
		}
	}

	return found;
}

/*
 * Search
 *
 * Locate the phone number in the given database.
 *
 *  -> db		Database object.
 * <-  dMH		Destination memory handle.
 * <-  dOffset		Destination offset.
 * 
 * Returns true if found, false otherwise.
 */
static Boolean
Search(DB *db, MemHandle *dMH, UInt16 *dOffset)
{
	UInt16 cnt=DBNumRecords(db), offset, idx;
	UInt16 *u;
	UInt8 *p;
	MemHandle mh;

	*dMH=NULL;
	for (idx=0; idx<cnt; idx++) {
		mh=DBGetRecord(db, idx);
		u=MemHandleLock(mh);
		if (*u==filter) {
			p=(UInt8 *)u;
			if ((offset=FindInRecord(p, MemHandleSize(mh)))>0) {
				*dMH=GetPersonData(p+offset);
				*dOffset=((*(p+offset+2))<<8)|(*(p+offset+3));
				MemHandleFree(mh);

				return (*dMH) ? true : false;
			}
		} else if (*u>filter) {
			MemHandleFree(mh);
			return false;
		}

		MemHandleFree(mh);
	}

	return false;
}

/*
 * FindInRecord
 *
 * Locate the phone number in this record.
 *
 *  -> rec		Record data.
 *  -> recLen		Record length.
 *
 * Returns the offset into this record if found, 0 otherwise.
 */
static UInt16
FindInRecord(UInt8 *rec, UInt32 recLen)
{
	UInt8 *recSave=rec, *recEnd=rec+recLen;
	UInt16 len;

	rec+=2;

	while (rec<recEnd) {
		len=(UInt16)*(rec+5);

		if (len==reversePhoneLen && (MemCmp(rec+6, reversePhone, reversePhoneLen)==0))
			return rec-recSave;

		rec+=(UInt32)(6+(UInt16)len);
	}
		
	return 0;
}

/*
 * GetPersonData
 *
 * Get the person data associated with this record.
 *
 *  -> rec		Record.
 *
 * Returns MemHandle or NULL.
 */
static MemHandle
GetPersonData(UInt8 *rec)
{
	UInt8 c=*(rec+4);
	Char dbName[dmDBNameLength];
	DB db;

	StrPrintF(dbName, "PFOC_pfoc index%c%c", hexChars[(c>>4)&0x0f], hexChars[c&0x0f]);
	DBInit(&db, dbName, rootPath);
	if (DBFindDatabase(&db)==FDFound) {
		if (DBOpen(&db, dmModeReadOnly, true)==true) {
			UInt16 recIdx=((*rec)<<8)|(*(rec+1));

			return DBGetRecord(&db, recIdx);
		}
	}

	return NULL;
}
