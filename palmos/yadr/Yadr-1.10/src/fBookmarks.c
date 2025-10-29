/*
 * fBookmarks.c
 */
#include "Include.h"

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static void FormBookmarksAndList(Boolean);
static void BuildDocumentsList(void);
static void UpdatePopup(void);
static void UpdateBookmarksList(Boolean);
static void DeleteAllBookmarks(void);
static Boolean EditBookmark(UInt16);
static Boolean EBEH(EventType *);

/* globals */
static MemHandle docListH;
static Char **docList;
static UInt16 returnToForm;
static Table tblBookmarks;
static Boolean bmChanged;
static Bookmark bm;

/*
 * DrawTable
 */
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	UInt16 pW=r->extent.x, rIdx=0, len;
	Int32 items=((Int32)tblBookmarks.top)+((Int32)row);
	DmOpenRef db=dbBookmarks.db;
	ListType *l=UIObjectGet(cBookmarksDocList);
	Int16 idx;
	Char *key;
	MemHandle rh;
	Boolean trunc;
	Bookmark *b;

	if ((idx=LstGetSelection(l))==noListSelection)
		return;

	key=LstGetSelectionText(l, idx);
	do {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmUnfiledCategory))==NULL)
			return;

		b=MemHandleLock(rh);
		if (StrCaselessCompare(key, b->docName)==0)
			items--;

		if (items>=0) {
			MemHandleUnlock(rh);
			rIdx++;
		}
	} while (items>=0);

	TblSetRowData(tbl, row, (UInt32)rIdx);
	WinEraseRectangle(r, 0);

	len=StrLen(b->bookmark);
	FntCharsInWidth(b->bookmark, &pW, &len, &trunc);
	WinDrawChars(b->bookmark, len, r->topLeft.x, r->topLeft.y);
	MemHandleUnlock(rh);
}

/*
 * fBookmarksRun
 *
 * Show the "Manage Bookmarks" form.
 *
 *  -> returnId		Return to form.
 */
void
fBookmarksRun(UInt16 returnId)
{
	if (DmNumRecordsInCategory(dbBookmarks.db, dmUnfiledCategory)==0) {
		FrmAlert(aNoBookmarks);
		return;
	}

	returnToForm=returnId;
	FrmGotoForm(fBookmarks);
}

/*
 * fBookmarksClose
 *
 * Close resources on form.
 */
void
fBookmarksClose(void)
{
	if (docListH) {
		MemHandleFree(docListH);
		docListH=NULL;
	}
}

/*
 * fBookmarksInit
 */
Boolean
fBookmarksInit(void)
{
	fBookmarksClose();

	UITableInit(&tblBookmarks, cBookmarksTable, cBookmarksScrollBar, DrawTable, style1, 0);

	FormBookmarksAndList(false);
	return true;
}

/*
 * fBookmarksEH
 */
Boolean
fBookmarksEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		return true;

	case popSelectEvent:
		if (ev->data.popSelect.listID==cBookmarksDocList) {
			if ((ev->data.popSelect.selection!=ev->data.popSelect.priorSelection) && (ev->data.popSelect.selection!=noListSelection)) {
				UpdateBookmarksList(true);
				tblBookmarks.top=0;
				UITableUpdateValues(&tblBookmarks, true);
			}
		}
		break;

	case tblSelectEvent:
		if (EditBookmark((UInt16)TblGetRowData(tblBookmarks.tbl, ev->data.tblSelect.row))) {
			if (DmNumRecordsInCategory(dbBookmarks.db, dmUnfiledCategory)>0)
				FormBookmarksAndList(true);
			else
				FrmGotoForm(returnToForm);
		}

		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cBookmarksDone:
			fBookmarksClose();
			FrmGotoForm(returnToForm);
			return true;
		case cBookmarksDeleteAll:
			if (FrmAlert(aDeleteAllBookmarks)==0) {
				DeleteAllBookmarks();
				if (DmNumRecordsInCategory(dbBookmarks.db, dmUnfiledCategory)>0)
					FormBookmarksAndList(true);
				else
					FrmGotoForm(returnToForm);
			}
			return true;
		}
		break;

	default:
		UITableEvents(&tblBookmarks, ev);
		break;
	}

	return false;
}

/*
 * FormBookmarksAndList
 *
 * Create the documents popup and bookmarks list.
 * 
 *  -> redraw		Redraw table.
 */
static void
FormBookmarksAndList(Boolean redraw)
{
	Int16 selection=noListSelection;
	ListType *l;
	UInt16 idx;

	BuildDocumentsList();
	l=UIObjectGet(cBookmarksDocList);
	if (doc) {
		for (idx=0; docList[idx]; idx++) {
			if (StrCaselessCompare(docList[idx], doc->title)==0) {
				selection=idx;
				break;
			}
		}
	}

	LstSetSelection(l, selection);
	UpdatePopup();
	UpdateBookmarksList(redraw);
}

/*
 * BuildDocumentsList
 *
 * (Re-)build the documents list.
 */
static void
BuildDocumentsList(void)
{
	DmOpenRef db=dbBookmarks.db;
	UInt32 blkSize=0;
	UInt16 rIdx=0, dCnt=0, cnt=0;
	ListType *l=UIObjectGet(cBookmarksDocList);
	Char lastDoc[DocNameLength];
	MemHandle rh;
	Bookmark *b;
	Char *str;

	*lastDoc=0;
	if (docListH) {
		MemHandleFree(docListH);
		docListH=NULL;
	}

	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmUnfiledCategory))==NULL)
			break;

		b=MemHandleLock(rh);
		if ((StrCaselessCompare(lastDoc, b->docName))) {
			StrCopy(lastDoc, b->docName);
			blkSize+=1+StrLen(b->docName);
			dCnt++;
		}

		MemHandleUnlock(rh);
		rIdx++;
	}

	if (blkSize&1)
		blkSize++;

	blkSize+=((dCnt+1)*sizeof(Char *));
	docListH=MemHandleNew(blkSize);
	ErrFatalDisplayIf(docListH==NULL, "(BuildDocumentsList) Out of memory");

	docList=MemHandleLock(docListH);
	MemSet(docList, blkSize, 0);

	str=((Char *)docList)+((dCnt+1)*sizeof(Char *));

	*lastDoc=0;
	rIdx=0;
	cnt=0;
	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmUnfiledCategory))==NULL)
			break;

		b=MemHandleLock(rh);
		if ((StrCaselessCompare(lastDoc, b->docName))) {
			StrCopy(lastDoc, b->docName);
			docList[cnt++]=str;
			StrCopy(str, b->docName);
			str+=1+StrLen(b->docName);
		}

		MemHandleUnlock(rh);
		rIdx++;
	}

	LstSetListChoices(l, docList, dCnt);
	if (dCnt<BookmarksDocListLength)
		LstSetHeight(l, dCnt);
	else
		LstSetHeight(l, BookmarksDocListLength);
}

/*
 * UpdatePopup
 *
 * Update documents popup.
 */
static void
UpdatePopup(void)
{
	ListType *l=UIObjectGet(cBookmarksDocList);
	Int16 idx;

	if ((idx=LstGetSelection(l))==noListSelection) {
		idx=0;
		LstSetSelection(l, 0);
	}

	CtlSetLabel(UIObjectGet(cBookmarksDocPopup), LstGetSelectionText(l, idx));
}

/*
 * UpdateBookmarksList
 *
 * Update the bookmarks list.
 *
 *  -> redraw		Redraw list (true = yes, false = no).
 */
static void
UpdateBookmarksList(Boolean redraw)
{
	UInt16 rIdx=0, bCnt=0;
	DmOpenRef db=dbBookmarks.db;
	ListType *l=UIObjectGet(cBookmarksDocList);
	Int16 idx;
	Char *key;
	MemHandle rh;
	Bookmark *b;

	if ((idx=LstGetSelection(l))==noListSelection)
		return;

	key=LstGetSelectionText(l, idx);
	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmUnfiledCategory))==NULL)
			break;

		b=MemHandleLock(rh);
		if (StrCaselessCompare(key, b->docName)==0)
			bCnt++;

		MemHandleUnlock(rh);
		rIdx++;
	}

	tblBookmarks.records=bCnt;
	UITableUpdateValues(&tblBookmarks, redraw);
}

/*
 * DeleteAllBookmarks
 *
 * Delete all bookmark for this document.
 */
static void
DeleteAllBookmarks(void)
{
	DmOpenRef db=dbBookmarks.db;
	ListType *l=UIObjectGet(cBookmarksDocList);
	UInt16 rIdx=0;
	MemHandle rh;
	Int16 r;
	Char *key;

	if ((r=LstGetSelection(l))==noListSelection)
		return;

	key=LstGetSelectionText(l, r);
	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmUnfiledCategory))==NULL)
			break;

		r=StrCaselessCompare(key, ((Bookmark *)(MemHandleLock(rh)))->docName);
		MemHandleUnlock(rh);

		if (!r)
			DmDeleteRecord(db, rIdx);
		else
			rIdx++;
	}
}

/*
 * EditBookmark
 *
 * Edit/Delete bookmark.
 *
 *  -> rIdx		Record index.
 *
 * Returns true if bookmark was changed or deleted, false otherwise.
 */
static Boolean
EditBookmark(UInt16 rIdx)
{
	FormSave frm;
	MemHandle mh;

	bmChanged=false;
	recordIdx=rIdx;
	UIFormPrologue(&frm, fEditBookmark, EBEH);

	mh=DBGetRecord(&dbBookmarks, rIdx);
	MemMove(&bm, MemHandleLock(mh), sizeof(Bookmark));
	MemHandleFree(mh);

	UIFieldSelectAll(cEditBookmarkName, bm.bookmark, true);

	if (UIFormEpilogue(&frm, NULL, cEditBookmarkCancel)==false)
		return false;

	return bmChanged;
}

/*
 * EBEH
 */
static Boolean
EBEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent) {
		Char *p;

		switch (ev->data.ctlSelect.controlID) {
		case cEditBookmarkOK:
			MemSet(bm.bookmark, BookmarkLength, 0);

			if ((p=UIFieldGetText(cEditBookmarkName)) && (StrLen(p)>0))
				StrCopy(bm.bookmark, p);
			else
				StrCopy(bm.bookmark, "Untitled Bookmark");

			DBSetRecord(&dbBookmarks, recordIdx, dmUnfiledCategory, &bm, sizeof(Bookmark), BookmarksSort);
			bmChanged=true;
			break;

		case cEditBookmarkDelete:
			if (FrmAlert(aDeleteBookmark)==0) {
				DmDeleteRecord(dbBookmarks.db, recordIdx);
				bmChanged=true;
			}
			break;
		}
	}

	return false;
}
