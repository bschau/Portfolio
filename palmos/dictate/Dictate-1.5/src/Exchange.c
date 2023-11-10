/*
 * fGroup.c
 */
#include "Include.h"

/*
 * GroupLookup
 *
 * Find group by name.
 *
 *  -> grpName		Group name.
 *
 * Returns record index of group (or dmMaxRecordIndex if not found).
 */
static UInt16
GroupLookup(Char *grpName)
{
	UInt16 rIdx=0, d1, d2;
	Int16 c;
	MemHandle rh;
	Char *r;

	if ((rIdx=DmFindSortPosition(dbSGroup.db, grpName, NULL, SFString, 0))!=0) {
		rIdx--;
		rh=DBGetRecord(&dbSGroup, rIdx);
		r=MemHandleLock(rh);
		c=TxtGlueCaselessCompare(grpName, StrLen(grpName), &d1, r, StrLen(r), &d2);
		MemHandleUnlock(rh);
		if (!c)
			return rIdx;
	}

	return dmMaxRecordIndex;
}

/*
 * ExchangeDelete
 *
 * Empty a given group (delete xrefs and sentences).
 *
 *  -> guid		Group ID.
 *  -> dbX		XRef db.
 *  -> dbS		Sentence db.
 */
void
ExchangeDelete(UInt32 guid, DB *dbX, DB *dbS)
{
	UInt16 rIdx;
	UInt32 suid;
	MemHandle mh;

	while ((rIdx=XRefIndex(dbX, guid, 0))!=dmMaxRecordIndex) {
		mh=DBGetRecord(dbX, rIdx);
		suid=((XRefRecord *)MemHandleLock(mh))->suid;
		MemHandleUnlock(mh);
		DmRemoveRecord(dbX->db, rIdx);

		if (DmFindRecordByID(dbS->db, suid, &rIdx)==errNone)
			DmRemoveRecord(dbS->db, rIdx);
	}
}

/*
 * ExchangeDBToStudent
 *
 * Transfer a group of sentences from the Teacher database to the
 * student realm.
 *
 *  -> guid		Group UID.
 *
 * Returns number of sentences imported.
 */
UInt16
ExchangeDBToStudent(UInt32 guid)
{
	UInt16 itemNum=0, trans=0, rIdx;
	MemHandle mh, rh;
	XRefRecord *xR;
	XRefRecord x;
	Char *p;

	if (DmFindRecordByID(dbTGroup.db, guid, &rIdx)!=errNone)
		return dmMaxRecordIndex;

	mh=DBGetRecord(&dbTGroup, rIdx);
	p=MemHandleLock(mh);

	if ((rIdx=GroupLookup(p))==dmMaxRecordIndex)
		x.guid=DBSetRecord(&dbSGroup, dmMaxRecordIndex, dmUnfiledCategory, p, StrLen(p)+1, SFString);
	else {
		DmRecordInfo(dbSGroup.db, rIdx, NULL, &x.guid, NULL);
		ExchangeDelete(x.guid, &dbSXref, &dbSSentence);
	}

	MemHandleUnlock(mh);
	while ((rIdx=XRefIndex(&dbTXref, guid, itemNum))!=dmMaxRecordIndex) {
		mh=DBGetRecord(&dbTXref, rIdx);
		xR=MemHandleLock(mh);
		if (DmFindRecordByID(dbTSentence.db, xR->suid, &rIdx)==errNone) {
			rh=DBGetRecord(&dbTSentence, rIdx);
			p=MemHandleLock(rh);
			x.suid=DBSetRecord(&dbSSentence, dmMaxRecordIndex, dmUnfiledCategory, p, StrLen(p)+1, SFString);
			MemHandleUnlock(rh);
			XRefInsert(&dbSXref, &x);
			trans++;
		}
		MemHandleUnlock(mh);
		itemNum++;
	}

	return trans;
}

/*
 * ExchangeIRToStudent
 *
 * Import sentences from IR.
 *
 * Returns number of sentences imported.
 */
UInt16
ExchangeIRToStudent(void)
{
	UInt16 sCnt=DmNumRecords(dbBeam.db), sNo, size, rIdx;
	MemHandle mh;
	UInt8 *p;
	XRefRecord x;

	for (sNo=0; sNo<sCnt; sNo++) {
		mh=DBGetRecord(&dbBeam, sNo);
		p=MemHandleLock(mh);

		size=(((UInt8)(*p++))<<8)&0xff00;
		size|=((UInt8)(*p++));
		if (!sNo) {
			if ((rIdx=GroupLookup(p))!=dmMaxRecordIndex) {
				DmRecordInfo(dbSGroup.db, rIdx, NULL, &x.guid, NULL);
				ExchangeDelete(x.guid, &dbSXref, &dbSSentence);
			} else
				x.guid=DBSetRecord(&dbSGroup, dmMaxRecordIndex, dmUnfiledCategory, p, size+1, SFString);
		}

		p+=size+1;
		size=(((UInt8)(*p++))<<8)&0xff00;
		size|=((UInt8)(*p++));
		x.suid=DBSetRecord(&dbSSentence, dmMaxRecordIndex, dmUnfiledCategory, p, size+1, NULL);
		MemHandleUnlock(mh);

		XRefInsert(&dbSXref, &x);
	}

	return sCnt;
}
