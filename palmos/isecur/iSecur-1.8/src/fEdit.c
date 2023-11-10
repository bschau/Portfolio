/*
 * fEdit.c
 */
#include "Include.h"

/* protos */
_FRM(static void SaveData(void));
_FRM(static void Load(UInt16));

/* globals */
static UInt16 fEditBand[]={ cEditFldTitle, cEditFldSecret, 0 };
extern UInt16 category, forceLoad;
extern DmResID iconID;
extern UInt8 startChar;
extern UInt16 libRefNum;
extern MemHandle aesData;
extern UInt16 recordIdx;
extern DmOpenRef dbRefIcons;
extern DB dbData;
extern State state;
extern Char catName1[dmCategoryLength];
extern Prefs prefs;

/*
 * fEditRun
 *
 * Setup for editing.
 *
 *  -> rIdx		Record Index (or dmMaxRecordIndex for new record).
 *  -> addChar		Start with this character for new records.
 *  -> forceLoadTmpl	Bring up the load template button?
 */
void
fEditRun(UInt16 rIdx, WChar addChar, Boolean forceLoadTmpl)
{
	forceLoad=forceLoadTmpl;
	startChar=(UInt8)addChar;
	recordIdx=rIdx;
	FrmGotoForm(fEdit);
}

/*
 * fEditInit
 */
Boolean
fEditInit(void)
{
	if (dbRefIcons) {
		UIObjectShow(cEditLblIcon);
		UIObjectShow(cEditIcon);
	}

	if (recordIdx==dmMaxRecordIndex) {
		if (startChar) {
			Char buffer[2];

			if (startChar>='a' && startChar<='z')
				buffer[0]=(startChar&~32);
			else
				buffer[0]=startChar;

			buffer[1]=0;

			UIFieldSetText(cEditFldTitle, buffer);
		}

		UIFieldFocus(cEditFldTitle);
		iconID=noIconSelected;
		category=state.category;
	} else {
		MemHandle mh=DBGetRecord(&dbData, recordIdx);
		RecordHdr *h;
		UInt8 *p;
		UInt16 attrs;

		h=MemHandleLock(mh);
		p=((UInt8 *)h)+sizeof(RecordHdr);

		if (h->flags&RHTemplate)
			CtlSetValue(UIObjectGet(cEditChkTemplate), 1);

		iconID=h->iconID;

		if (h->flags&RHHasTitle) {
			UInt32 len=StrLen(p)+1;

			UIFieldSetText(cEditFldTitle, p);
			if (len&1)
				len++;
			p+=len;
		}

		if (h->flags&RHHasSecret) {
			UInt32 hLen=MemHandleSize(mh)-(p-(UInt8 *)h), idx;
			MemHandle rh;
			UInt8 *e;

			rh=MemHandleNew(hLen);
			ErrFatalDisplayIf(rh==NULL, "(fEditInit) Out of memory");
			e=MemHandleLock(rh);
			MemMove(e, p, hLen);

			for (idx=0; idx<hLen; idx+=32) {
				AESDecrypt(&e[idx], &e[idx]);
				AESDecrypt(&e[idx+16], &e[idx+16]);
			}

			UIFieldSetText(cEditFldSecret, e);
			MemHandleFree(rh);
		}

		MemHandleUnlock(mh);

		UIObjectShow(cEditDelete);

		DmRecordInfo(dbData.db, recordIdx, &attrs, NULL, NULL);
		category=attrs&dmRecAttrCategoryMask;
	}

	UIFieldUpdateScrollBar(cEditFldSecret, cEditScrBarSecret);

	if (category==dmAllCategories)
		category=dmUnfiledCategory;

	UIPopupSet(dbData.db, category, catName1, cEditPopup);
	return true;
}

/*
 * fEditEH
 */
Boolean
fEditEH(EventType *ev)
{
	RectangleType r;
	UInt16 rIdx;
	UInt32 uid;

	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case frmOpenEvent:
		if (forceLoad)
			CtlHitControl(UIObjectGet(cEditLoad));

	case frmUpdateEvent:
		if (!forceLoad)
			FrmDrawForm(currentForm);

		if (dbRefIcons) {
			FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cEditIcon), &r);
			WinEraseRectangle(&r, 0);

			fMainDrawIcon(iconID, r.topLeft.x, r.topLeft.y);
			WinDrawRectangleFrame(popupFrame, &r);
		}
		return true;

	case frmSaveEvent:
		if (appStopped)
			SaveData();

		UIFieldClear(cEditFldTitle);
		UIFieldClear(cEditFldSecret);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cEditOK:
			if (!(prefs.flags&PFlgSecretStay))
				state.category=category;

			DBSetRecord(&dbData, recordIdx, category, (CtlGetValue(UIObjectGet(cEditChkTemplate))) ? RHTemplate : 0, iconID, UIFieldGetText(cEditFldTitle), UIFieldGetText(cEditFldSecret), DBSort);
		case cEditCancel:	/* FALL-THRU */
			FrmGotoForm(fMain);
			return true;
		case cEditLoad:
			if (fTemplateRun(category, &rIdx)==true) {
				Load(rIdx);
				UIFormRedraw();
			}
			return true;
		case cEditDelete:
			if (FrmAlert(aDeleteRecord)==0) {
				DmDeleteRecord(dbData.db, recordIdx);
				FrmGotoForm(fMain);
			}
			return true;
		case cEditGenerate:
			fPwdGenRequester();
			return true;
		case cEditPopup:
			CategorySelect(dbData.db, currentForm, cEditPopup, cEditList, false, &category, catName1, 1, 0);
			return true;
		case cEditIcon:
			fIconRun(&iconID);
			UIFormRedraw();
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mEditCreate:
			state.category=category;
			uid=DBSetRecord(&dbData, recordIdx, state.category, (CtlGetValue(UIObjectGet(cEditChkTemplate))) ? RHTemplate : 0, iconID, UIFieldGetText(cEditFldTitle), UIFieldGetText(cEditFldSecret), DBSort);
			if (DBFindRecordByID(&dbData, uid, &rIdx)==errNone) {
				fLetCreateRun(rIdx);
				UIFormRedraw();
			} else
				FrmAlert(aNoRecByID);

			return true;
		case mEditLoad:
			if (fTemplateRun(category, &rIdx)==true) {
				Load(rIdx);
				UIFormRedraw();
			}
			return true;
		case mEditDelete:
			if (recordIdx!=dmMaxRecordIndex) {
				if (FrmAlert(aDeleteRecord)==0) {
					DmDeleteRecord(dbData.db, recordIdx);
					FrmGotoForm(fMain);
				}
			}
			return true;
		case mEditGenerate:
			fPwdGenRequester();
			return true;

		default:
			break;
		}
		break;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cEditFldSecret, cEditScrBarSecret);
		UIFieldRefocus(fEditBand, ev->data.keyDown.chr);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, cEditFldSecret, cEditScrBarSecret);
		break;
	}

	return false;
}

/*
 * SaveData
 *
 * Save the data and make note of this ..
 */
static void
SaveData(void)
{
	Char *t=UIFieldGetText(cEditFldTitle), *s=UIFieldGetText(cEditFldSecret);
	UInt16 tLen=0, sLen=0;
	UInt32 uid;

	if (t)
		tLen=StrLen(t);

	if (s)
		sLen=StrLen(s);

	if ((tLen==0) && (sLen==0))
		return;

	uid=DBSetRecord(&dbData, recordIdx, state.category, (CtlGetValue(UIObjectGet(cEditChkTemplate))) ? RHTemplate : 0, iconID, t, s, DBSort);

	PMSetPref(&uid, sizeof(UInt32), PrfSaveData, false);
}

/*
 * Load
 *
 * Load the given template.
 *
 *  -> tIdx		Template Index.
 */
static void
Load(UInt16 tIdx)
{
	MemHandle mh=DBGetRecord(&dbData, tIdx), rh=NULL, th=NULL, sh=NULL;
	Char *origText=UIFieldGetText(cEditFldSecret);
	Char *title=NULL;
	UInt8 *pText=NULL, *p;
	UInt32 origLen=0;
	RecordHdr *h;
	UInt16 attrs;

	if (origText) {
		if (!(origLen=StrLen(origText)))
			origText=NULL;
	}

	h=MemHandleLock(mh);
	p=((UInt8 *)h)+sizeof(RecordHdr);

	if (h->flags&RHHasTitle) {
		UInt32 len=StrLen(p)+1;

		title=p;
		if (len&1)
			len++;

		p+=len;
	}

	if (h->flags&RHHasSecret) {
		UInt32 hLen=MemHandleSize(mh)-(p-(UInt8 *)h), idx;
		UInt8 *e;

		rh=MemHandleNew(hLen);
		ErrFatalDisplayIf(rh==NULL, "(Load) Out of memory");
		pText=MemHandleLock(rh);
		MemMove(pText, p, hLen);
		e=pText;

		for (idx=0; idx<hLen; idx+=32) {
			AESDecrypt(e, e);
			AESDecrypt(e+16, e+16);
			e+=32;
		}
	}

	if ((!title) && (!pText)) {
		if (rh) {
			MemSet(MemHandleLock(rh), MemHandleSize(rh), 0);
			MemHandleFree(rh);
		}

		MemHandleUnlock(mh);
		return;
	}

	if (fKeywordsOpen(title, pText)==true) {
		if (fKeywordsRun(title, pText, &th, &sh)==false) {
			if (th)
				MemHandleFree(th);

			if (sh)
				MemHandleFree(sh);

			if (rh)
				MemHandleFree(rh);

			MemHandleUnlock(mh);
			return;
		}
	}

	iconID=h->iconID;
	if (th) {
		UIFieldSetText(cEditFldTitle, MemHandleLock(th));
		MemSet(MemHandleLock(th), MemHandleSize(th), 0);
		MemHandleFree(th);
	} else if (title)
		UIFieldSetText(cEditFldTitle, title);

	if (sh || pText) {
		UInt8 *dTxt;

		if (sh)
			dTxt=MemHandleLock(sh);
		else
			dTxt=pText;

		if (origText) {
			UInt16 choice=FrmAlert(aAddOrReplace);

			if (choice==2)
				UIFieldSetText(cEditFldSecret, dTxt);
			else {
				FieldType *fld=UIObjectGet(cEditFldSecret);

				if (choice==0)
					FldSetInsertionPoint(fld, 0);
				else
					FldSetInsertionPoint(fld, origLen+1);

				FldInsert(fld, dTxt, StrLen(dTxt));
			}
		} else
			UIFieldSetText(cEditFldSecret, dTxt);

		if (sh) {
			MemSet(MemHandleLock(sh), MemHandleSize(sh), 0);
			MemHandleFree(sh);
		}
	}

	if (rh) {
		MemSet(MemHandleLock(rh), MemHandleSize(rh), 0);
		MemHandleFree(rh);
	}

	MemHandleUnlock(mh);

	if (prefs.flags&PFlgTemplateInherit) {
		DmRecordInfo(dbData.db, tIdx, &attrs, NULL, NULL);
		category=attrs&dmRecAttrCategoryMask;
		UIPopupSet(dbData.db, category, catName1, cEditPopup);
	}

	UIFieldUpdateScrollBar(cEditFldSecret, cEditScrBarSecret);
}
