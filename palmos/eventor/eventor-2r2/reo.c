/*
**	reo.c
**
**	Reorganization routines. Used to upgrade from one version of the
**	database to another.
*/
#include "eventor.h"

/*
**	Reo1To2
**
**	Upgrade to v2 of database.
**
**	 -> newDb	New database object.
**	 -> oldDb	Old database object.
**
**	Returns true if all ok, false otherwise.
*/
Boolean
Reo1To2(DB *newDb, DB *oldDb)
{
	DmOpenRef ndb=newDb->db, odb=oldDb->db;
	Char catName[dmCategoryLength];
	UInt16 idx, attrs, recIdx;
	TEvent e;
	UInt8 *p;
	MemHandle mh;

	/* copy categories */
	for (idx=1; idx<dmRecNumCategories; idx++) {
		CategoryGetName(odb, idx, catName);
		CategorySetName(ndb, idx, catName);
	}

	/* copy and convert */
	for (idx=0; idx<DmNumRecords(odb); idx++) {
		if ((mh=DmQueryRecord(odb, idx))==NULL)
			return false;

		DmRecordInfo(odb, idx, &attrs, NULL, NULL);
		if ((attrs&dmRecAttrDelete)==dmRecAttrDelete)
			continue;

		MemSet(&e, sizeof(TEvent), 0);
		p=MemHandleLock(mh);
		e.date=*(UInt16 *)p;
		p+=sizeof(UInt16);
		e.flags=*(UInt16 *)p;
		p+=sizeof(UInt16);
		StrNCopy(e.event, p, EventLength);
		MemHandleUnlock(mh);

		recIdx=dmMaxRecordIndex;
		if ((mh=DmNewRecord(ndb, &recIdx, sizeof(TEvent)))==NULL)
			return false;

		DmWrite(MemHandleLock(mh), 0, &e, sizeof(TEvent));
		MemHandleUnlock(mh);

		attrs|=dmRecAttrDirty;
		DmSetRecordInfo(ndb, recIdx, &attrs, NULL);
		DmReleaseRecord(ndb, recIdx, true);
	}

	/* ok, almost done - sort ... */
	DmInsertionSort(ndb, (DmComparF *)EventorCompareFunction, 0);
	return true;
}
