/*
 * XRef.c
 */
#include "Include.h"

/*
 * XRefIndex
 *
 * Find record index of xref given a group UID.
 *
 *  -> xDB		XRef Database.
 *  -> guid		Group UID.
 *  -> offset		Offset (dmMaxRecordIndex to search all).
 *
 * Returns the index or dmMaxRecordIndex if not found.
 */
UInt16
XRefIndex(DB *xDB, UInt32 guid, Int32 offset)
{
	DmOpenRef db=xDB->db;
	UInt16 lastIdx=dmMaxRecordIndex, rIdx=0;
	MemHandle rh;
	XRefRecord *r;

	do {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmAllCategories))==NULL)
			return dmMaxRecordIndex;

		r=MemHandleLock(rh);
		if (r->guid==guid) {
			lastIdx=rIdx;
			offset--;
		}

		MemHandleUnlock(rh);
		rIdx++;
	} while (offset>=0);

	return lastIdx;
}

/*
 * XRefInsert
 *
 * Insert xref in database.
 *
 *  -> xDB		XRef Database.
 *  -> xRef		XRef.
 */
void
XRefInsert(DB *xDB, XRefRecord *xRef)
{
	UInt16 rIdx=dmMaxRecordIndex;
	DmOpenRef db=xDB->db;
	MemHandle rh;

	rh=DmNewRecord(db, &rIdx, sizeof(XRefRecord));
	ErrFatalDisplayIf(rh==NULL, "(XRefInsert) Device is full.");

	DmWrite(MemHandleLock(rh), 0, xRef, sizeof(XRefRecord));
	MemHandleUnlock(rh);

	DmReleaseRecord(db, rIdx, true);
}

/*
 * XRefCount
 *
 * Count no.# of XRefs with this group uid.
 *
 *  -> xDB		XRef Database.
 *  -> guid		Group UID.
 *
 * Returns the count.
 */
UInt16
XRefCount(DB *xDB, UInt32 guid)
{
	DmOpenRef db=xDB->db;
	UInt16 rIdx=0, cnt=0;
	MemHandle rh;
	XRefRecord *r;

	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmAllCategories))==NULL)
			break;

		r=MemHandleLock(rh);
		if (r->guid==guid)
			cnt++;
		MemHandleUnlock(rh);
		rIdx++;
	}

	return cnt;
}

/*
 * XRefDelete
 *
 * Delete a xref given a group UID and offset.
 *
 *  -> xDB		XRef Database.
 *  -> guid		Group UID.
 *  -> offset		Offset.
 */
void
XRefDelete(DB *xDB, UInt32 guid, Int32 offset)
{
	DmOpenRef db=xDB->db;
	UInt16 lastIdx=dmMaxRecordIndex, rIdx=0;
	MemHandle rh;
	XRefRecord *r;

	do {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmAllCategories))==NULL)
			break;

		r=MemHandleLock(rh);
		if (r->guid==guid) {
			lastIdx=rIdx;
			offset--;
		}

		MemHandleUnlock(rh);
		rIdx++;
	} while (offset>=0);

	if (lastIdx!=dmMaxRecordIndex)
		DmDeleteRecord(db, lastIdx);
}

/*
 * XRefDeleteAllGuid
 *
 * Delete all records matching given guid.
 *
 *  -> xDB		XRef Database.
 *  -> guid		Group UID.
 */
void
XRefDeleteAllGuid(DB *xDB, UInt32 guid)
{
	DmOpenRef db=xDB->db;
	UInt16 rIdx=0;
	MemHandle rh;
	XRefRecord *r;

	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmAllCategories))==NULL)
			break;

		r=MemHandleLock(rh);
		if (r->guid==guid) {
			MemHandleUnlock(rh);
			DmDeleteRecord(db, rIdx);
		} else {
			MemHandleUnlock(rh);
			rIdx++;
		}
	}
}

/*
 * XRefDeleteAllSuid
 *
 * Delete all records matching given suid.
 *
 *  -> xDB		XRef Database.
 *  -> suid		Sentence UID.
 */
void
XRefDeleteAllSuid(DB *xDB, UInt32 suid)
{
	DmOpenRef db=xDB->db;
	UInt16 rIdx=0;
	MemHandle rh;
	XRefRecord *r;

	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmAllCategories))==NULL)
			break;

		r=MemHandleLock(rh);
		if (r->suid==suid) {
			MemHandleUnlock(rh);
			DmDeleteRecord(db, rIdx);
		} else {
			MemHandleUnlock(rh);
			rIdx++;
		}
	}
}

/*
 * XRefSwap
 *
 * Swap two XRefs.
 *
 *  -> xDB		XRef Database.
 *  -> guid		Group ID.
 *  -> fromIdx		Source Index (in list).
 *  -> toIdx		Destination Index (in list).
 */
void
XRefSwap(DB *xDB, UInt32 guid, UInt16 fromIdx, UInt16 toIdx)
{
	UInt16 fIdx=XRefIndex(xDB, guid, fromIdx), tIdx=XRefIndex(xDB, guid, toIdx);
	UInt32 fGuid, fSuid, tGuid, tSuid;
	MemHandle mF, mT;
	XRefRecord *rF, *rT;

	if (fIdx==dmMaxRecordIndex || tIdx==dmMaxRecordIndex)
		return;

	mF=DBGetRecord(xDB, fIdx);
	rF=MemHandleLock(mF);
	fGuid=rF->guid;
	fSuid=rF->suid;

	mT=DBGetRecord(xDB, tIdx);
	rT=MemHandleLock(mT);
	tGuid=rT->guid;
	tSuid=rT->suid;

	DmWrite(rF, 0, &tGuid, sizeof(UInt32));
	DmWrite(rF, sizeof(UInt32), &tSuid, sizeof(UInt32));
	DmWrite(rT, 0, &fGuid, sizeof(UInt32));
	DmWrite(rT, sizeof(UInt32), &fSuid, sizeof(UInt32));

	MemHandleUnlock(mT);
	MemHandleUnlock(mF);

	DmReleaseRecord(xDB->db, fIdx, true);
	DmReleaseRecord(xDB->db, tIdx, true);
}

/*
 * XRefExists
 *
 * Check to see if the Xref already exists.
 *
 *  -> xDB		XRef Database.
 *  -> x		Xref.
 *
 * Return true if Xref exists, false otherwise.
 */
Boolean
XRefExists(DB *xDB, XRefRecord *x)
{
	DmOpenRef db=xDB->db;
	UInt32 guid=x->guid, suid=x->suid;
	UInt16 rIdx=0;
	MemHandle rh;
	XRefRecord *r;

	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmAllCategories))==NULL)
			break;

		r=MemHandleLock(rh);
		if (r->guid==guid && r->suid==suid) {
			MemHandleUnlock(rh);
			return true;
		} else {
			MemHandleUnlock(rh);
			rIdx++;
		}
	}

	return false;
}
