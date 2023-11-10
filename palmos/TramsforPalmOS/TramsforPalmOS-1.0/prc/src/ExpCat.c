/*
 * ExpCat.c
 */
#include "Include.h"

/*
 * ExpCatGetIndex
 *
 * Given a Exp Cat code, return index.
 *
 *  -> ecCode		Exp Cat code.
 *
 * Returns index or dmMaxRecordIndex if not found.
 */
UInt16
ExpCatGetIndex(Char *ecCode)
{
	UInt16 cnt=DmNumRecordsInCategory(dbExpCat.db, dmAllCategories);
	UInt16 idx, len;
	MemHandle mh;
	Char *p;

	for (idx=0; idx<cnt; idx++) {
		mh=DBGetRecord(&dbExpCat, idx);
		p=MemHandleLock(mh);
		len=(UInt16)*p;
		if (MemCmp(p+len+3, ecCode, 4)==0) {
			MemHandleUnlock(mh);
			return idx;
		}

		MemHandleUnlock(mh);
	}

	return dmMaxRecordIndex;
}

/*
 * ExpCatGetCategory
 *
 * Given a Exp Cat code, return category string.
 *
 *  -> ecCode		Exp Cat code.
 *
 * Returns string or NULL if not found.
 * NOTE: Caller must do a MemPtr free on the string when finished.
 */
Char *
ExpCatGetCategory(Char *ecCode)
{
	UInt16 cnt=DmNumRecordsInCategory(dbExpCat.db, dmAllCategories);
	UInt16 idx, len;
	MemHandle mh, rh;
	Char *p, *c;

	for (idx=0; idx<cnt; idx++) {
		mh=DBGetRecord(&dbExpCat, idx);
		p=MemHandleLock(mh);
		len=(UInt16)*p;
		if (MemCmp(p+len+3, ecCode, 4)==0) {
			rh=MemHandleNew(len+1);
			ErrFatalDisplayIf(rh==NULL, "(ExpCatGetCategory) Out of memory");

			c=MemHandleLock(rh);
			MemMove(c, p+2, len+1);
			MemHandleUnlock(mh);
			return c;
		}

		MemHandleUnlock(mh);
	}

	return NULL;
}
