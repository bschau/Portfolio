/*
 * History.c
 */
#include "Include.h"

/*
 * HistorySort
 */
Int16
HistorySort(void *r1, void *r2, Int16 u1, SortRecordInfoType *u2, SortRecordInfoType *u3, MemHandle u4)
{
	return StrCaselessCompare(((History *)r1)->docName, ((History *)r2)->docName);
}

/*
 * HistoryOpen
 *
 * Open history if indicated so by the preferences.
 *
 * Returns true if successful, false otherwise.
 */
Boolean
HistoryOpen(void)
{
	DBInit(&dbHistory, DBNameHistory, NULL);
	if (prefs.flags&PFHistory) {
		DBCreateDatabase(DBNameHistory, defCatHistory);

		return DBOpen(&dbHistory, dmModeReadWrite, true);
	}

	return true;
}

/*
 * HistoryClose
 *
 * Close history.
 */
void
HistoryClose(void)
{
	DBClose(&dbHistory);
}

/*
 * HistoryAdd
 *
 * Add a document to the history list if it is not already present.
 *
 *  -> docName		Document name.
 * <-  uid		Where to store UID.
 *
 * Returns true if record was added, false otherwise (it exists!)
 */
Boolean
HistoryAdd(Char *docName, UInt32 *uid)
{
	if (prefs.flags&PFHistory) {
		MemHandle mh;
		History h;
		UInt16 rIdx;

		MemSet(&h, sizeof(History), 0);
		StrCopy(h.docName, docName);

		if ((rIdx=DmFindSortPosition(dbHistory.db, &h, NULL, HistorySort, 0))>0) {
			History *r;

			rIdx--;
			mh=DBGetRecord(&dbHistory, rIdx);
			r=MemHandleLock(mh);

			if (StrCaselessCompare(h.docName, r->docName)==0) {
				UInt16 attr;

				DmRecordInfo(dbHistory.db, rIdx, &attr, NULL, NULL);
				r->last=TimGetSeconds();
				*uid=DBSetRecord(&dbHistory, rIdx, attr&dmRecAttrCategoryMask, r, sizeof(History), NULL);
				MemHandleFree(mh);
				return true;
			}
			MemHandleFree(mh);
		}
		
		if (DmNumRecords(dbHistory.db)<dmMaxRecordIndex) {
			h.opened=TimGetSeconds();
			h.last=h.opened;

			*uid=DBSetRecord(&dbHistory, dmMaxRecordIndex, HCatOpened, &h, sizeof(History), HistorySort);
			return true;
		}
	}

	return false;
}

/*
 * HistoryMarkAsRead
 *
 * Mark the document given by the UID as read.
 *
 *  -> uid		Document.
 */
void
HistoryMarkAsRead(UInt32 uid)
{
	if (prefs.flags&PFHistory) {
		UInt16 rIdx;

		if (DmFindRecordByID(dbHistory.db, uid, &rIdx)==0) {
			UInt16 attr;

			DmRecordInfo(dbHistory.db, rIdx, &attr, NULL, NULL);
			attr&=~dmRecAttrCategoryMask;
			attr|=HCatRead;
			DmSetRecordInfo(dbHistory.db, rIdx, &attr, NULL);
		}
	}
}

/*
 * HistoryClear
 *
 * Clear history list for category.
 *
 *  -> cat		Category.
 */
void
HistoryClear(UInt16 cat)
{
	UInt16 cnt=DmNumRecordsInCategory(dbHistory.db, cat), rIdx;

	while (cnt>0) {
		rIdx=DmPositionInCategory(dbHistory.db, 0, cat);
		if (DmSeekRecordInCategory(dbHistory.db, &rIdx, 0, dmSeekForward, cat)!=errNone)
			break;

		DmDeleteRecord(dbHistory.db, rIdx);
	}
}
