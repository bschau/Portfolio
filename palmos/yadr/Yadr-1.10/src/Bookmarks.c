/*
 * Bookmarks.c
 */
#include "Include.h"

/* globals */
static Char *txtAdd="Add Bookmark ...";

/*
 * BookmarksSort
 */
Int16
BookmarksSort(void *r1, void *r2, Int16 u1, SortRecordInfoType *u2, SortRecordInfoType *u3, MemHandle u4)
{
	Int16 r;

	if ((r=StrCaselessCompare(((Bookmark *)r1)->docName, ((Bookmark *)r2)->docName)))
		return r;

	return StrCaselessCompare(((Bookmark *)r1)->bookmark, ((Bookmark *)r2)->bookmark);
}

/*
 * BookmarksOpen
 *
 * Open bookmarks.
 *
 * Returns true if successful, false otherwise.
 */
Boolean
BookmarksOpen(void)
{
	DBInit(&dbBookmarks, DBNameBookmarks, NULL);

	return DBOpen(&dbBookmarks, dmModeReadWrite, true);
}

/*
 * BookmarksClose
 *
 * Close bookmarks.
 */
void
BookmarksClose(void)
{
	DBClose(&dbBookmarks);
}

/*
 * BookmarksBuildList
 *
 * Build list of bookmarks.
 *
 *  -> docName		Document name (get from doc->title).
 *  -> id		Object ID.
 *  -> addAdd		Add the "Add Bookmarks" text.
 *  -> lines		Lines in list.
 *  -> sBuffer		Stringbuffer to use.
 */
MemHandle
BookmarksBuildList(Char *docName, UInt16 id, Boolean addAdd, UInt16 lines, StringBuffer *sBuffer)
{
	ListType *l=UIObjectGet(id);
	UInt16 rIdx=0, bCnt=0;
	DmOpenRef db=dbBookmarks.db;
	MemHandle rh;
	Bookmark *b;
	
	SBReset(sBuffer); 

	if (addAdd) {
		SBAppend(sBuffer, txtAdd, StrLen(txtAdd)+1);
		bCnt++;
	}

	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmUnfiledCategory))==NULL)
			break;

		b=MemHandleLock(rh);
		if (StrCaselessCompare(docName, b->docName)==0) {
			SBAppend(sBuffer, b->bookmark, StrLen(b->bookmark)+1);
			bCnt++;
		}

		MemHandleUnlock(rh);
		rIdx++;
	}

	rh=SysFormPointerArrayToStrings(SBGetData(sBuffer, &rIdx), bCnt);
	ErrFatalDisplayIf(rh==NULL, "(BookmarksBuildList) Out of memory");

	if (addAdd) {
		if (bCnt<lines)
			LstSetHeight(l, bCnt);
		else
			LstSetHeight(l, lines);
	}

	LstSetTopItem(l, 0);
	LstSetSelection(l, noListSelection);
	LstSetListChoices(l, MemHandleLock(rh), bCnt);

	return rh;
}

/*
 * BookmarksAdd
 *
 * Add a bookmark to the current document.
 *
 *  -> docName		Document name.
 *  -> pos		Position in document.
 *  -> name		Bookmark name.
 */
void
BookmarksAdd(Char *docName, UInt32 pos, Char *name)
{
	Bookmark b;

	MemSet(&b, sizeof(Bookmark), 0);
	StrCopy(b.docName, docName);
	StrCopy(b.bookmark, name);
	b.position=pos;

	DBSetRecord(&dbBookmarks, dmMaxRecordIndex, dmUnfiledCategory, &b, sizeof(Bookmark), BookmarksSort);
}

/*
 * BookmarksGetPosition
 *
 * Returns position based on list index.
 *
 *  -> docName		Document name.
 *  -> idx		List index.
 *
 * Returns position.
 */
UInt32
BookmarksGetPosition(Char *docName, Int16 idx)
{
	DmOpenRef db=dbBookmarks.db;
	UInt32 position=0;
	UInt16 rIdx=0;
	MemHandle rh;
	Bookmark *b;

	do {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmUnfiledCategory))==NULL)
			return 0;

		b=MemHandleLock(rh);
		if (StrCaselessCompare(docName, b->docName)==0)
			idx--;

		position=b->position;
		MemHandleUnlock(rh);
		rIdx++;
	} while (idx>=0);

	return position;
}

/*
 * BookmarksDeleteAll
 *
 * Delete all bookmark for this document.
 *
 *  -> docName		Document name.
 */
void
BookmarksDeleteAll(Char *docName)
{
	DmOpenRef db=dbBookmarks.db;
	UInt16 rIdx=0;
	MemHandle rh;
	Int16 r;

	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmUnfiledCategory))==NULL)
			break;

		r=StrCaselessCompare(docName, ((Bookmark *)(MemHandleLock(rh)))->docName);
		MemHandleUnlock(rh);

		if (!r)
			DmDeleteRecord(db, rIdx);
		else
			rIdx++;
	}
}
