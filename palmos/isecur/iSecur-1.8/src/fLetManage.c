/*
 * fLetManage.c
 */
#include "Include.h"

/* protos */
_UTL(static Int16 CLSort(void *, void *, Int32));
_UTL(static void CreateList(void));
_UTL(static void DrawList(Int16, RectangleType *, Char **));
_UTL(static Boolean EH(EventType *));
_UTL(static void Delete(UInt16));
_UTL(static void SetupView(Boolean, Int16));
_UTL(static void PruneLets(void));
_UTL(static Boolean GetSLetID(UInt16, LocalID, UInt32 *));
_UTL(static UInt16 FindByLetter(Char));

/* globals */
extern DB dbData;
extern MemHandle sletH;
extern Prefs prefs;
static UInt16 letCnt;

/*
 * CLSort
 */
static Int16
CLSort(void *r1, void *r2, Int32 u)
{
	return StrCaselessCompare(((SysDBListItemType *)r1)->name, ((SysDBListItemType *)r2)->name);
}

/*
 * CreateList
 *
 * Create the iSecurLets list.
 *
 * Returns number of elements on the list.
 */
static void
CreateList(void)
{
	if (sletH) {
		MemHandleFree(sletH);
		sletH=NULL;
	}

	SysCreateDataBaseList((UInt32)'slet', CRID, &letCnt, &sletH, false);

	if (letCnt>0) {
		SysQSort(MemHandleLock(sletH), letCnt, sizeof(SysDBListItemType), CLSort, 0);
		MemHandleUnlock(sletH);
	}
}

/*
 * DrawList
 */
static void
DrawList(Int16 itemNum, RectangleType *rct, Char **unused)
{
	UInt16 pW=148, len;
	Boolean fitAll;
	SysDBListItemType *s;

	if ((!sletH) || (itemNum>letCnt))
		return;

	s=MemHandleLock(sletH);
	WinEraseRectangle(rct, 0);

	len=StrLen(s[itemNum].name);
	FntCharsInWidth(s[itemNum].name, &pW, &len, &fitAll);
	WinDrawChars(s[itemNum].name, len, rct->topLeft.x, rct->topLeft.y);
	MemHandleUnlock(sletH);
}


/*
 * fLetManageRun
 *
 * Run the Manage iSecurLets dialog.
 */
void
fLetManageRun(void)
{
	FormSave frm;

	sletH=NULL;
	CreateList();

	UIFormPrologue(&frm, fLetManage, EH);

	if (letCnt) {
		SetupView(false, noListSelection);
		UIObjectShow(cLetManageList);
		UIObjectShow(cLetManagePrune);
	} else
		UIObjectShow(cLetManageLbl);

	UIFormEpilogue(&frm, NULL, 0);
	if (sletH)
		MemHandleFree(sletH);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	switch (ev->eType) {
	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cLetManagePrune) {
			if (FrmAlert(aPruneLet)==0)
				PruneLets();

			return true;
		}
		break;

	case lstSelectEvent:
		if (ev->data.lstSelect.selection!=noListSelection) {
			if (FrmAlert(aLetDelete)==0)
				Delete((UInt16)ev->data.lstSelect.selection);

			return true;
		}
		break;

	case keyDownEvent:
		if (UIListKeyHandler(ev, cLetManageList, prefs.flags&PFlg5Way)==false) {
			if (TxtGlueCharIsPrint(ev->data.keyDown.chr)) {
				UIListUpdateFocus(cLetManageList, FindByLetter(ev->data.keyDown.chr));
				return true;
			}
		} else
			return true;

	default:	/* FALL-THRU */
		return UIListKeyHandler(ev, cLetManageList, prefs.flags&PFlg5Way);
	}

	return false;
}

/*
 * Delete
 *
 * Delete the iSecurLet.
 *
 *  -> no		Index into list.
 */
static void
Delete(UInt16 no)
{
	SysDBListItemType *s;

	if (!sletH)
		return;

	s=MemHandleLock(sletH);

	if (DmDeleteDatabase(s[no].cardNo, s[no].dbID)!=errNone) {
		MemHandleUnlock(sletH);
		FrmAlert(aLetDeleteError);
		return;
	}

	CreateList();
	if (letCnt==0) {
		RectangleType r;
	
		UIObjectHide(cLetManageList);

		RctSetRectangle(&r, 4, 15, 154, 121);
		WinEraseRectangle(&r, 0);
		UIObjectShow(cLetManageLbl);
	} else {
		if (no>1)
			no--;

		SetupView(true, no);
	}
}

/*
 * SetupView
 *
 * Setup the items list.
 *
 *  -> redraw		Redraw list.
 *  -> active		Active item (or noListSelection).
 */
static void
SetupView(Boolean redraw, Int16 active)
{
	UIListInit(cLetManageList, DrawList, letCnt, active);
	if (redraw)
		LstDrawList(UIObjectGet(cLetManageList));
}

/*
 * PruneLets
 *
 * Remove iSecurLets which points to non-existing secrets.
 */
static void
PruneLets(void)
{
	SysDBListItemType *s=MemHandleLock(sletH);
	UInt16 deleted=0;
	UInt16 idx, rIdx;
	UInt32 id;

	for (idx=0; idx<letCnt; idx++) {
		if (GetSLetID(s[idx].cardNo, s[idx].dbID, &id)==true) {
			if (DBFindRecordByID(&dbData, id, &rIdx)==dmErrUniqueIDNotFound) {
				if (DmDeleteDatabase(s[idx].cardNo, s[idx].dbID)==errNone)
					deleted++;
			}
		}
	}
	
	MemHandleUnlock(sletH);

	if (deleted) {
		Char buffer[16];

		CreateList();

		if (letCnt==0) {
			UIObjectHide(cLetManageList);
			UIObjectHide(cLetManagePrune);
			UIObjectShow(cLetManageLbl);
		} else
			SetupView(false, noListSelection);

		UIFormRedraw();

		StrPrintF(buffer, "%u", deleted);
		FrmCustomAlert(aPruneStatus, buffer, NULL, NULL);
	} else 
		FrmAlert(aPruneNoneDeleted);
}

/*
 * GetSLetID
 *
 * Return the ID stored in the iSecurLet.
 *
 *  -> cardNo		Card no of iSecurLet.
 *  -> dbID		DB ID of iSecurLet.
 * <-  id		Where to store unique id.
 *
 * Returns true if id was found, false otherwise.
 */
static Boolean
GetSLetID(UInt16 cardNo, LocalID dbID, UInt32 *id)
{
	Boolean ret=false;
	DmOpenRef db;
	UInt16 idx;
	MemHandle mh;

	if ((db=DmOpenDatabase(cardNo, dbID, dmModeReadOnly))==NULL)
		return false;

	if ((idx=DmFindResource(db, constantRscType, iSecurLetResID, NULL))!=dmMaxRecordIndex) {
		if ((mh=DmGetResourceIndex(db, idx))!=NULL) {
			*id=*(UInt32 *)(MemHandleLock(mh));
			MemHandleUnlock(mh);
			DmReleaseResource(mh);
			ret=true;
		}
	}

	DmCloseDatabase(db);
	return ret;
}

/**
 * Find the closest matching entry by letter.
 *
 * @param ltr Letter (or number :-)
 * @return index into list.
 */
static UInt16
FindByLetter(Char ltr)
{
	UInt16 lIdx=0, idx;
	SysDBListItemType *s;
	Char str[2];
	Int16 ret;

	if ((!sletH) || (letCnt<1))
		return 0;

	str[0]=ltr;
	str[1]=0;

	s=MemHandleLock(sletH);

	for (idx=0; idx<letCnt; idx++) {
		if ((ret=StrNCaselessCompare(str, s[idx].name, 1))<=0) {
			lIdx=idx;
			break;
		}
	}

	MemHandleUnlock(sletH);
	return lIdx;
}
