/*
 * NameSearch.c
 */
#include "Include.h"

/* protos */
static MemHandle FindFirst(Int32 *);
static UInt32 FindPerson(MemHandle, UInt32);
static Char *UncrunchPhoneNibble(Char *, Char *, UInt8);

/* globals */
static MemHandle givenNameH, surNameH;
static Char *givenName, *surName;

/*
 * NSOpen
 *
 * Initialize search engine.
 */
void
NSOpen(void)
{
	MemSet(&dbNameSearch, sizeof(DB), 0);
	givenNameH=MemHandleNew(SearchNameLength+2);
	surNameH=MemHandleNew(SearchNameLength+2);
	ErrFatalDisplayIf(givenNameH==NULL || surNameH==NULL, "(NSOpen) Out of memory.");

	givenName=MemHandleLock(givenNameH);
	surName=MemHandleLock(surNameH);
}

/*
 * NSClose
 *
 * Shutdown search engine.
 */
void
NSClose(void)
{
	MemHandleFree(surNameH);
	MemHandleFree(givenNameH);
	DBClose(&dbNameSearch);
}

/*
 * NSSetup
 *
 * Initialize search engine.
 *
 *  -> fN	First Name.
 *  -> lN	Last Name.
 */
void
NSSetup(Char *fN, Char *lN)
{
	Char dbName[dmDBNameLength];
	Char *given, *sur;
	UInt16 len;
	UInt8 c;

	if (lN!=NULL) {
		sur=lN;
		given=fN;
	} else {
		sur=fN;
		given=NULL;
	}
	
	if (given) {
		len=StrLen(given);
		MemMove(givenName, given, len);
	} else
		len=0;
	givenName[len]='\x00';

	len=StrLen(sur);
	MemMove(surName, sur, len);
	surName[len]='\x00';

	c=*surName;

	StrPrintF(dbName, "PFOC_pfoc index%c%c", hexChars[(c>>4)&0x0f], hexChars[c&0x0f]);
	DBInit(&dbNameSearch, dbName, rootPath);
	if (DBFindDatabase(&dbNameSearch)==FDFound) {
		if (DBOpen(&dbNameSearch, dmModeReadOnly, true)==true) { 
			MemHandle mh;

			if ((mh=FindFirst(&recordIdx))==NULL)
				FrmAlert(aNoMatch);
			else {
				MemHandleFree(mh);
				FrmGotoForm(fNameSearch);
			}
		}
	} else
		FrmAlert(aNoMatch);
}

/*
 * FindFirst
 *
 * Find the first matching record in this database.
 *
 * <-  rIdx		Record index.
 * 
 * Returns MemHandle to record if found (and updates rIdx), NULL otherwise.
 */
static MemHandle
FindFirst(Int32 *rIdx)
{
	UInt16 cnt=DBNumRecords(&dbNameSearch), lo=0, hi, mid;
	MemHandle mh;
	Char *p;
	Int16 cV;

	hi=cnt-1;

	*rIdx=0;
	while (lo<=hi) {
		mid=(UInt16)(((UInt32)lo+(UInt32)hi)/2);

		mh=DBGetRecord(&dbNameSearch, mid);

		p=MemHandleLock(mh);
		if ((cV=StrCaselessCompare(p, surName))==0) {
			MemHandleUnlock(mh);
			if (*givenName) {
				if (FindPerson(mh, 0)>0) {
					*rIdx=mid;
					return mh;
				}

				MemHandleFree(mh);
				return NULL;
			}

			*rIdx=mid;
			return mh;
		}
		MemHandleFree(mh);

		if (cV>0) {
			hi=mid-1;
		} else {
			lo=mid+1;
		}
	}

	return NULL;
}

/*
 * NSFindPerson
 *
 * Find a person in the given chunk.
 *
 *  -> p		Chunk.
 *  -> sIdx		Start position.
 *  -> eIdx		End index.
 *
 * Returns index into surname record or 0 if not found.
 */
UInt32
NSFindPerson(Char *p, UInt32 sIdx, UInt32 eIdx)
{
	UInt32 nLen=StrLen(givenName);
	Char *s;

	if (!sIdx) {
		for (s=p; *s; s++)
			sIdx++;

		sIdx++;
	}

	while (sIdx<eIdx) {
		if (StrNCompareAscii(givenName, p+sIdx+3, nLen)==0)
			return sIdx;

		sIdx=NSNextPerson(p, sIdx);
	}

	return 0;
}

/*
 * FindPerson
 *
 * Given a Surname record, locate person.
 *
 *  -> mh		Surname record.
 *  -> sIdx		Start position.
 *
 * Returns index into surname record or 0 if not found.
 */
static UInt32
FindPerson(MemHandle mh, UInt32 sIdx)
{
	sIdx=NSFindPerson(MemHandleLock(mh), sIdx, MemHandleSize(mh));
	MemHandleUnlock(mh);
	return sIdx;
}

/*
 * NSNextPerson
 *
 * Fast forward to next person.
 *
 *  -> src		Source block.
 *  -> idx		Start index.
 *
 * Returns start index of next person (next person may be '\x00')
 */
UInt32
NSNextPerson(Char *src, UInt32 idx)
{
	idx+=2;				/* Manager ID */

	idx+=(UInt32)src[idx]+1;	/* Given Name */
	idx+=(UInt32)src[idx]+1;	/* Phone */
	idx+=(UInt32)src[idx]+1;	/* Mobile */

	idx+=2;				/* Location */

	for (src+=idx; *src; src++)	/* Email */
		idx++;

	return ++idx;			/* Term byte */
}

/*
 * NSContainsPerson
 *
 * Does the record at recordIdx+dir contain the person.
 *
 *  -> dir		Record index.
 *
 * Returns true if person is found, false otherwise.
 */
Boolean
NSContainsPerson(Int32 dir)
{
	Boolean ret=false;
	Int32 idx=recordIdx+dir;
	MemHandle mh;
	Char *p;

	if ((idx<0) || (idx>DBNumRecords(&dbNameSearch)))
		return false;

	mh=DBGetRecord(&dbNameSearch, (UInt16)idx);

	p=MemHandleLock(mh);
	if (StrNCompareAscii(p, surName, StrLen(surName))==0) {
		if (*givenName) {
			if (FindPerson(mh, 0)>0)
				ret=true;
		} else
			ret=true;
	}
	MemHandleFree(mh);
	return ret;
}

/*
 * NSCountNames
 *
 * Count names matching the search criterias.
 *
 * Returns number of matches.
 */
UInt16
NSCountNames(void)
{
	UInt16 matches=0;
	UInt32 idx, recSize, nLen;
	MemHandle mh;
	Char *p;

	if (*givenName)
		nLen=StrLen(givenName);
	else
		nLen=0;

	mh=DBGetRecord(&dbNameSearch, recordIdx);

	recSize=MemHandleSize(mh);
	p=MemHandleLock(mh);
	idx=StrLen(p)+1;
	while (idx<recSize) {
		if (nLen) {
			if (StrNCompareAscii(p+idx+3, givenName, nLen)==0)
				matches++;
		} else
			matches++;

		idx=NSNextPerson(p, idx);
	}

	MemHandleFree(mh);
	return matches;
}

/*
 * NSUncrunchPhone
 *
 * Uncrunch a phone number.
 *
 * <-  dst		Destination buffer (of MaxPhoneLength) in size!
 *  -> src		Source buffer.
 *
 * Returns start of phone number.
 */
Char *
NSUncrunchPhone(Char *dst, Char *src)
{
	Char *d=dst+MaxPhoneLength-1;
	UInt16 len=(UInt16)*src;
	UInt8 b, c;

	*d=0;
	src++;

	if (len>0) {
		while (d>dst && len>0) {
			b=*src++;
			if ((c=((b>>4)&0x0f))==12)
				break;

			d=UncrunchPhoneNibble(dst, d, c);

			if ((c=(b&0x0f))==12)
				break;

			d=UncrunchPhoneNibble(dst, d, c);
		
			len--;
		}

		if (d>dst)
			*--d='+';
	} else
		*--d='-';

	return d;
}

/*
 * UncrunchPhoneNibble
 *
 * Uncrunch a phone nibble.
 *
 *  -> g		Guard.
 *  -> d		Destination.
 *  -> n		Nibble.
 *
 * Return new destination.
 */
static Char *
UncrunchPhoneNibble(Char *g, Char *d, UInt8 n)
{
	if (n==11) {
		if (d>g) {
			UInt16 t=d-g;

			if (t>6)
				t=6;

			d-=t;
			MemMove(d, " ext. ", 6);
		}
	} else {
		if (d>g)
			*--d=n+'0';
	}

	return d;
}

/*
 * NSUppercaseWords
 *
 * Upper-case first letter in each word.
 *
 *  -> src		Source string.
 *
 * Returns changed string. src string is not touched. Caller is responsible
 * for freeing the new string.
 */
Char *
NSUppercaseWords(Char *src)
{
	Boolean upperCase=true;
	Int32 sLen=StrLen(src);
	MemHandle mh;
	Char *dst, *p;
	Char c;

	mh=MemHandleNew(sLen+1);
	ErrFatalDisplayIf(mh==NULL, "(UCWords) Out of memory.");

	dst=MemHandleLock(mh);
	p=dst;
	for (p=dst; *src; p++) {
		c=*src++;

		if (c<' ') {
			c=' ';
			upperCase=true;
		} else if (c==' ') {
			upperCase=true;
		} else if (c=='-') {
			upperCase=true;
		} else if (upperCase==true) {
			if (c>='a')
				c-='a'-'A';

			upperCase=false;
		}

		*p=c;
	}
	*p='\x00';

	return dst;
}
