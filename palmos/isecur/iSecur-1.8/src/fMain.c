/*
 * fMain.c
 */
#include "Include.h"

/* protos */
_FRM(static void DrawList(Int16, RectangleType *, Char **));
_FRM(static void SetupView(Boolean, Int16));
_FRM(static UInt16 CountRecords(void));
_FRM(static UInt16 SeekToRecord(UInt16));
_FRM(static void Beam(void));
_FRM(static Err BeamWriter(const void *, UInt32 *, void *));

/* globals */
extern Int16 mainLstIndex;
extern UInt16 libRefNum;
extern MemHandle aesData, sha256Data;
extern UI *ui;
extern Prefs prefs;
extern State state;
extern DmOpenRef dbRefIcons;
extern Char catName[dmCategoryLength], catName1[dmCategoryLength];
extern DB dbData;
extern UInt8 gKey[AESKeyLength];
extern Char gPwd[];

/*
 * DrawList
 */
static void
DrawList(Int16 itemNum, RectangleType *rct, Char **unused)
{
	UInt16 rIdx;
	MemHandle mh;
	RecordHdr *h;

	if ((rIdx=SeekToRecord(itemNum))==dmMaxRecordIndex)
		return;

	WinEraseRectangle(rct, 0);

	mh=DBGetRecord(&dbData, rIdx);
	h=MemHandleLock(mh);

	fMainDrawIcon(h->iconID, rct->topLeft.x, rct->topLeft.y+1);

	if (h->flags&RHHasTitle) {
       		UInt16 pW=rct->extent.x-14, len;
		FontID fID;
		Boolean trunc;
		Char *p;

		if (h->flags&RHTemplate)
			fID=FntSetFont(boldFont);
		else
			fID=FntSetFont(stdFont);

		p=((Char *)h)+sizeof(RecordHdr);
		len=StrLen(p);
		FntCharsInWidth(p, &pW, &len, &trunc);
		WinDrawChars(p, len, rct->topLeft.x+14, rct->topLeft.y);

		FntSetFont(fID);
	}

	MemHandleUnlock(mh);
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	ListType *l=UIObjectGet(cMainLstFilter);

	UIPopupSet(dbData.db, state.category, catName1, cMainPopup);

	LstSetSelection(l, (Int16)state.filter);
	CtlSetLabel(UIObjectGet(cMainPopFilter), LstGetSelectionText(l, (Int16)state.filter));

	SetupView(false, mainLstIndex);
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	Boolean edited;
	UInt16 category;

	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case frmOpenEvent:
	case frmUpdateEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainNew:
			fEditRun(dmMaxRecordIndex, 0, false);
			SetupView(true, LstGetSelection(UIObjectGet(cMainSecretsList)));
			return true;

		case cMainTmpl:
			fEditRun(dmMaxRecordIndex, 0, true);
			SetupView(true, LstGetSelection(UIObjectGet(cMainSecretsList)));
			return true;

		case cMainPopup:
			category=state.category;
			edited=CategorySelect(dbData.db, currentForm, cMainPopup, cMainList, true, &state.category, catName1, 1, 0);
			if (edited || (category!=state.category))
				SetupView(true, 0);

			return true;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.listID==cMainLstFilter) {
			if ((ev->data.popSelect.selection!=ev->data.popSelect.priorSelection) && (ev->data.popSelect.selection!=noListSelection)) {
				state.filter=ev->data.popSelect.selection;
				SetupView(true, 0);
			}
		}
		break;

	case lstSelectEvent:
		if (ev->data.lstSelect.selection!=noListSelection) {
			if ((category=SeekToRecord(ev->data.lstSelect.selection))!=dmMaxRecordIndex) {
				mainLstIndex=LstGetSelection(UIObjectGet(cMainSecretsList));
				PMOpenSecret(category);
				return true;
			}
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			fAboutRun();
			return true;
		case mMainChange:
			fMainChangePassword();
			return true;
		case mMainPasswordHint:
			fPasswordHintRun();
			return true;
		case mMainPreferences:
			fPreferencesRun();
			return true;
		case mMainBeam:
			Beam();
			return true;
		case mMainManage:
			fLetManageRun();
			return true;
		case mMainNew:
			fEditRun(dmMaxRecordIndex, 0, false);
			SetupView(true, LstGetSelection(UIObjectGet(cMainSecretsList)));
			return true;
		case mMainNewTmpl:
			fEditRun(dmMaxRecordIndex, 0, true);
			SetupView(true, LstGetSelection(UIObjectGet(cMainSecretsList)));
			return true;
		case mMainImport:
			fIERun(true);
			SetupView(true, LstGetSelection(UIObjectGet(cMainSecretsList)));
			return true;
		case mMainExport:
			fIERun(false);
			SetupView(true, LstGetSelection(UIObjectGet(cMainSecretsList)));
			return true;

		}
		break;

	case keyDownEvent:
		if (UIListKeyHandler(ev, cMainSecretsList, prefs.flags&PFlg5Way)==false) {
			if (TxtGlueCharIsPrint(ev->data.keyDown.chr)) {
				if (prefs.flags&PFlgNewSecret) {
					fEditRun(dmMaxRecordIndex, ev->data.keyDown.chr, false);
					SetupView(true, LstGetSelection(UIObjectGet(cMainSecretsList)));
				} else
					UIListUpdateFocus(cMainSecretsList, fMainFindByLetter(ev->data.keyDown.chr, state.filter, state.category));
				return true;
			}
		} else
			return true;

		return false;
	default:
		break;
	}

	return false;
}

/*
 * SetupView
 *
 * Setup the main list.
 *
 *  -> redraw		Redraw list.
 *  -> active		Active item (or noListSelection).
 */
static void
SetupView(Boolean redraw, Int16 active)
{
	UIListInit(cMainSecretsList, DrawList, CountRecords(), active);
	if (redraw)
		LstDrawList(UIObjectGet(cMainSecretsList));
}

/*
 * fMainChangePassword
 *
 * Change master password.
 */
void
fMainChangePassword(void)
{
	LocalID appInfoID;
	AppInfoBlock *a;

	if ((fPasswordRun(PwdDialogConfirm, NULL, 0, 1, true, DBVERSION)==false) || appStopped)
		return;

	appInfoID=DmGetAppInfoID(dbData.db);
	ErrFatalDisplayIf(appInfoID==NULL, "(ChangePassword) appInfoID == NULL ... nederen!");
	a=MemLocalIDToLockedPtr(appInfoID, dbData.card);
	SetPassword(a, gPwd, 0);
	MemPtrUnlock(a);

	FrmAlert(aPasswordChanged);

	if (FrmAlert(aSetNewHint)==0)
		fPasswordHintRun();
}

/*
 * CountRecords
 *
 * Count number of records goverend by filter and category.
 *
 * Returns number of records.
 */
static UInt16
CountRecords(void)
{
	UInt16 seek=0, cnt=0, rIdx=0;
	DmOpenRef db=dbData.db;
	MemHandle mh;

	if (!state.filter)
		return DmNumRecordsInCategory(db, state.category);

	if (state.filter==2)
		seek=RHTemplate;

	while ((mh=DmQueryNextInCategory(db, &rIdx, state.category))) {
		rIdx++;

		if (((((RecordHdr *)(MemHandleLock(mh)))->flags)&RHTemplate)==seek)
			cnt++;

		MemHandleUnlock(mh);
	}

	return cnt;
}

/*
 * SeekToRecord
 *
 * Seek to the Xth record governed by filter and category.
 *
 *  -> X		Record to seek to.
 *
 * Returns index of record or dmMaxRecordIndex if out of range.
 */
static UInt16
SeekToRecord(UInt16 X)
{
	DmOpenRef db=dbData.db;
	UInt16 rIdx=0, seek=0;
	MemHandle mh;

	if (!state.filter) {
		if (DmSeekRecordInCategory(db, &rIdx, X, dmSeekForward, state.category)!=errNone)
			return dmMaxRecordIndex;

		return rIdx;
	}

	if (state.filter==2)
		seek=RHTemplate;

	while ((mh=DmQueryNextInCategory(db, &rIdx, state.category))) {
		if (((((RecordHdr *)(MemHandleLock(mh)))->flags)&RHTemplate)==seek) {
			if (!X) {
				MemHandleUnlock(mh);
				return rIdx;
			} else
				X--;
		}

		MemHandleUnlock(mh);
		rIdx++;
	}

	return dmMaxRecordIndex;
}

/*
 * Beam
 *
 * Beam database (if possible).
 */
static void
Beam(void)
{
	ExgSocketType sck;
	UInt16 attr;
	Err err;

	if ((DmDatabaseInfo(dbData.card, dbData.id, NULL, &attr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone) || (attr&dmHdrAttrCopyPrevention)) {
		FrmAlert(aCopyPrevBitSet);
		return;
	}

	MemSet(&sck, sizeof(ExgSocketType), 0);
	sck.description="iSecur Database";
	sck.target=(UInt32)CRID;
	sck.name=DBNameData;
	sck.type="iSec";
	if ((err=ExgPut(&sck))==errNone) {
		err=ExgDBWrite(BeamWriter, &sck, NULL, dbData.id, dbData.card);
		err=ExgDisconnect(&sck, err);
	}

	if (err!=errNone)
		FrmAlert(aIRSendError);
}

/*
 * BeamWriter
 *
 * Beam callback.
 */
static Err
BeamWriter(const void *src, UInt32 *len, void *sck)
{
	Err err;

	*len=ExgSend((ExgSocketType *)sck, src, *len, &err);
	return err;
}

/*
 * fMainDrawIcon
 *
 * Draw icon at x,y.
 *
 *  -> id		Icon ID.
 *  -> x		X.
 *  -> y		Y.
 */
void
fMainDrawIcon(UInt16 id, UInt16 x, UInt16 y)
{
	UInt16 iIdx;

	if ((id==noIconSelected) || (dbRefIcons==NULL))
		return;


	if ((iIdx=DmFindResource(dbRefIcons, bitmapRsc, id, NULL))!=dmMaxRecordIndex) {
		MemHandle bh;

		if ((bh=DmGetResourceIndex(dbRefIcons, iIdx))) {
			WinDrawBitmap(MemHandleLock(bh), x, y);
			MemHandleUnlock(bh);
			DmReleaseResource(bh);
		}
	}
}

/**
 * Find the closest matching entry by letter.
 *
 * @param ltr Letter (or number :-)
 * @param filter Filter by (all, secrets, templates).
 * @param cat Category.
 * @return index into list.
 */
UInt16
fMainFindByLetter(Char ltr, UInt16 filter, UInt16 cat)
{
	DmOpenRef db=dbData.db;
	UInt16 rIdx=0, seek=(filter==2 ? RHTemplate : 0), lIdx=0, max, attrs;
	RecordHdr *r;
	MemHandle mh;
	Char str[2];
	Int16 ret;

	str[0]=ltr;
	str[1]=0;

	max=DmNumRecordsInCategory(db, dmAllCategories);

	for (rIdx=0; rIdx<max; rIdx++) {
		DmRecordInfo(db, rIdx, &attrs, NULL, NULL);
		if (attrs&dmRecAttrDelete)
			continue;

		if ((cat==dmAllCategories) || ((attrs&dmRecAttrCategoryMask)==cat)) {
			mh=DmQueryRecord(db, rIdx);
			ErrFatalDisplayIf(mh==NULL, "(FindByLetter) Cannot query record");

			r=MemHandleLock(mh);
			if (((!filter) || ((r->flags&RHTemplate)==seek))) {

				if ((ret=StrNCaselessCompare(str, ((Char *)r)+sizeof(RecordHdr), 1))<=0) {
					MemHandleUnlock(mh);
					return lIdx;
				}
				lIdx++;
			}
			MemHandleUnlock(mh);
		}
	}

	return lIdx;
}
