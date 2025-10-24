/**
 * Import/Export
 */
#include "Include.h"

/* protos */
_FRM(static void DrawList(Int16, RectangleType *, Char **));
_FRM(static void SetupView(Boolean, Int16));
_FRM(static void Mark(Boolean));
_FRM(static UInt16 FindByLetter(Char));
_FRM(static void Import(void));
_FRM(static Boolean ISEH(EventType *));
_FRM(static void Export(void));
_FRM(static Boolean ESEH(EventType *));

/* globals */
static Char *chkUncheck="\x00", *chkCheck="\x01";
extern Char name[dmDBNameLength], catName[dmCategoryLength], catName1[dmCategoryLength];
extern DB dbData, dbMemo;
extern State state;
extern Prefs prefs;
extern UInt16 rounds, libRefNum;
extern Boolean isImport;
extern MemHandle aesData;
extern DmOpenRef dbRefIcons;
extern DmResID iconID;
static DmOpenRef srcDB, dstDB;
static UInt16 category, bitCnt;
static MemHandle bitMapH;
static UInt8 *bitMap;

/*
 * DrawList
 */
static void
DrawList(Int16 itemNum, RectangleType *rct, Char **unused)
{
	UInt16 item=0, x=rct->topLeft.x, w=rct->extent.x, idx, bit, len;
	MemHandle mh;
	FontID fID;
	Boolean fitAll;
	Char *p;

	WinEraseRectangle(rct, 0);

	if (DmSeekRecordInCategory(srcDB, &item, itemNum, dmSeekForward, category)!=errNone)
		return;

	if ((mh=DmQueryRecord(srcDB, item))==NULL)
		return;

	fID=FntSetFont(symbol11Font);
	idx=item/8;
	bit=1<<(item&7);

	if (bitMap[idx]&bit)
		WinDrawChars(chkCheck, 1, x, rct->topLeft.y);
	else
		WinDrawChars(chkUncheck, 1, x, rct->topLeft.y);

	x+=14;
	w-=14;

	if (!isImport) {
		RecordHdr *h=MemHandleLock(mh);

		fMainDrawIcon(h->iconID, x, rct->topLeft.y+1);

		x+=14;
		w-=14;

		if (h->flags&RHTemplate)
			FntSetFont(boldFont);
		else
			FntSetFont(stdFont);

		if (h->flags&RHHasTitle)
			p=((Char *)h)+sizeof(RecordHdr);
		else
			p=NULL;
	} else {
		p=MemHandleLock(mh);
		FntSetFont(stdFont);
	}

	if (p) {
		for (; *p && *p<=' '; p++);
		if (*p) {
			len=StrLen(p);
			FntCharsInWidth(p, &w, &len, &fitAll);
			WinDrawChars(p, len, x, rct->topLeft.y);
		}
	}

	FntSetFont(fID);

	if (mh)
		MemHandleUnlock(mh);
}

/**
 * Run Import/export.
 *
 * @param ii Set to true to handle import, false is export.
 */
void
fIERun(Boolean ii)
{
	isImport=ii;

	if (dbMemo.db==NULL) {
		DBInit(&dbMemo, "MemoDB");
		if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
			return;
	}

	if (ii) {
		srcDB=dbMemo.db;
		if (!(rounds=DmNumRecordsInCategory(srcDB, dmAllCategories))) {
			FrmAlert(aNothingToImport);
			return;
		}
		dstDB=dbData.db;
		category=state.memoCat;
		SysStringByIndex(strIEAction, 0, name, sizeof(name));
	} else {
		srcDB=dbData.db;
		if (!(rounds=DmNumRecordsInCategory(srcDB, dmAllCategories))) {
			FrmAlert(aNothingToExport);
			return;
		}
		dstDB=dbMemo.db;
		category=state.category;
		SysStringByIndex(strIEAction, 1, name, sizeof(name));
	}

	if (rounds>bitCnt) {
		if (bitMapH)
			MemHandleFree(bitMapH);

		bitCnt=((rounds/8)+1)*8;

		bitMapH=MemHandleNew(bitCnt);
		ErrFatalDisplayIf(bitMapH==NULL, "(fIEInit) Out of memory");
		bitMap=MemHandleLock(bitMapH);
	}
	MemSet(bitMap, bitCnt, 0);

	FrmGotoForm(fIE);
}

/**
 * Close called from PilotMain::Stop.
 */
void
fIEClose(void)
{
	if (bitMapH)
		MemHandleFree(bitMapH);

	DBClose(&dbMemo);
}

/**
 * Initialize form.
 */
Boolean
fIEInit(void)
{
	UIPopupSet(srcDB, category, catName1, cIEPopup);

	SetupView(false, noListSelection);
	if (isImport)
		UIObjectShow(cIEImport);
	else
		UIObjectShow(cIEExport);

	FrmSetTitle(currentForm, name);
	return true;
}

/**
 * fIEEH
 */
Boolean
fIEEH(EventType *ev)
{
	Boolean edited;
	UInt16 cat;

	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case frmOpenEvent:
	case frmUpdateEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cIEDone:
			FrmGotoForm(fMain);
			return true;
		case cIEAll:
			Mark(true);
			return true;
		case cIENone:
			Mark(false);
			return true;
		case cIEPopup:
			cat=category;
			edited=CategorySelect(srcDB, currentForm, cIEPopup, cIEList, true, &category, catName1, 1, 0);
			if (edited || (cat!=category))
				SetupView(true, 0);

			return true;
		case cIEImport:
			for (cat=0; cat<bitCnt; cat++) {
				if (bitMap[cat]) {
					Import();
					LstDrawList(UIObjectGet(cIEItems));
					return true;
				}
			}

			FrmAlert(aImpNothingSelected);
			return true;
		case cIEExport:
			for (cat=0; cat<bitCnt; cat++) {
				if (bitMap[cat]) {
					Export();
					LstDrawList(UIObjectGet(cIEItems));
					return true;
				}
			}

			FrmAlert(aExpNothingSelected);
			return true;
		}
		break;

	case lstSelectEvent:
		if (ev->data.lstSelect.selection!=noListSelection) {
			UInt16 item=0;

			if (DmSeekRecordInCategory(srcDB, &item, ev->data.lstSelect.selection, dmSeekForward, category)==errNone) {
				UInt16 idx=item/8, bit=1<<(item&7);

				if (bitMap[idx]&bit)
					bitMap[idx]&=~bit;
				else
					bitMap[idx]|=bit;

				LstDrawList(UIObjectGet(cIEItems));
			}
			return true;
		}
		break;

	case keyDownEvent:
		if (UIListKeyHandler(ev, cIEItems, prefs.flags&PFlg5Way)==false) {
			if (TxtGlueCharIsPrint(ev->data.keyDown.chr)) {
				UIListUpdateFocus(cIEItems, FindByLetter(ev->data.keyDown.chr));
				return true;
			}
		} else
			return true;
	default:
		break;
	}

	return false;
}

/**
 * Setup the main list.
 *
 * @param redraw Redraw list.
 * @param active Active item (or noListSelection).
 */
static void
SetupView(Boolean redraw, Int16 active)
{
	UInt16 cnt;

	if (isImport)
		cnt=DmNumRecordsInCategory(dbMemo.db, category);
	else
		cnt=DmNumRecordsInCategory(dbData.db, category);

	UIListInit(cIEItems, DrawList, cnt, active);
	if (redraw)
		LstDrawList(UIObjectGet(cIEItems));
}

/**
 * Mark or unmark all items in this category.
 *
 * @param m Mark if true, unmark if false.
 */
static void
Mark(Boolean m)
{
	UInt16 idx=0, item, bit, attrs;

	for (item=0; item<rounds; idx++) {
		DmRecordInfo(srcDB, idx, &attrs, NULL, NULL);
		if (attrs&dmRecAttrDelete)
			continue;

		if ((category==dmAllCategories) || ((attrs&dmRecAttrCategoryMask)==category)) {
			attrs=idx/8;
			bit=1<<(idx&7);

			if (m)
				bitMap[attrs]|=bit;
			else
				bitMap[attrs]&=~bit;

		}

		item++;
	}

	LstDrawList(UIObjectGet(cIEItems));
}

/**
 * Find the closest matching entry by letter.
 *
 * @param ltr Letter.
 * @return index into list.
 */
static UInt16
FindByLetter(Char ltr)
{
	UInt16 lIdx=0, rIdx=0, idx, attrs;
	Char str[2];
	MemHandle mh;
	RecordHdr *hdr;
	Char *p;

	str[0]=ltr;
	str[1]=0;

	for (idx=0; idx<rounds; rIdx++) {
		DmRecordInfo(srcDB, rIdx, &attrs, NULL, NULL);
		if (attrs&dmRecAttrDelete)
			continue;

		if ((category==dmAllCategories) || ((attrs&dmRecAttrCategoryMask)==category)) {
			mh=DmQueryRecord(srcDB, idx);
			ErrFatalDisplayIf(mh==NULL, "(FindByLetter) Cannot query record");

			if (!isImport) {
				hdr=MemHandleLock(mh);
				if (hdr->flags&RHHasTitle)
					p=((Char *)hdr)+sizeof(RecordHdr);
				else
					p=NULL;
			} else
				p=MemHandleLock(mh);

			if (p) {
				for (; *p && *p<=' '; p++);

				if (StrNCaselessCompare(str, p, 1)<=0) {
					MemHandleUnlock(mh);
					return lIdx;
				}
			}

			lIdx++;
			MemHandleUnlock(mh);
		}
		idx++;
	}

	return lIdx;
}

/**
 * Show Import settings dialog and optionally import items.
 */
static void
Import(void)
{
	UInt16 cnt=0, flags=0, rIdx=0, attrs, curIdx, idx, bit;
	Char buffer[12];
	UInt32 size;
	FormSave frm;
	MemHandle mh, rh;
	Char *d, *t, *s;
	EventType ev;

	UIFormPrologue(&frm, fImportSettings, ISEH);

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=frmUpdateEvent;
	ev.data.frmLoad.formID=currentFormID;
	EvtAddEventToQueue(&ev);

	UIPopupSet(dstDB, state.impCat, catName, cImportSettingsPopup);

	if (state.impExpFlags&STIEFImpDelete)
		CtlSetValue(UIObjectGet(cImportSettingsDelete), 1);

	if (state.impExpFlags&STIEFImpTemplate)
		CtlSetValue(UIObjectGet(cImportSettingsTemplate), 1);

	iconID=noIconSelected;
	if (dbRefIcons) {
		UIObjectShow(cImportSettingsIcon);
		UIObjectShow(cImportSettingsLblIcon);
	}

	if (UIFormEpilogue(&frm, NULL, cImportSettingsCancel)==false)
		return;

	if (state.impExpFlags&STIEFImpTemplate)
		flags=RHTemplate;

	for (curIdx=0; curIdx<rounds; rIdx++) {
		DmRecordInfo(srcDB, rIdx, &attrs, NULL, NULL);
		if (attrs&dmRecAttrDelete)
			continue;

		idx=rIdx/8;
		bit=1<<(rIdx&7);
		if (bitMap[idx]&bit) {
			mh=DmQueryRecord(srcDB, rIdx);
			ErrFatalDisplayIf(mh==NULL, "(Import) Failed to get record from Memo");

			size=MemHandleSize(mh);
			rh=MemHandleNew(size+1);
			ErrFatalDisplayIf(rh==NULL, "(Import) Out of memory");

			d=MemHandleLock(rh);
			MemMove(d, MemHandleLock(mh), size);
			MemHandleUnlock(mh);
			d[size]=0;

			for (; *d && *d<=' '; d++);
			if (*d) {
				t=d;
				for (; *d && *d!='\n'; d++);
				if (*d) {
					*d++=0;
					s=d;
					if (StrLen(s)>SecretLength)
						s[SecretLength]=0;
				} else
					s=NULL;

				DBSetRecord(&dbData, dmMaxRecordIndex, state.impCat, flags, iconID, t, s, NULL);
			}
			MemHandleFree(rh);

			if (state.impExpFlags&STIEFImpDelete) 
				DmDeleteRecord(srcDB, rIdx);

			cnt++;
		}
		curIdx++;
	}
	DmQuickSort(dbData.db, DBSort, 0);
	MemSet(bitMap, bitCnt, 0);
	StrPrintF(buffer, "%u", cnt);
	FrmCustomAlert(aImpImported, buffer, NULL, NULL);

	if (state.impExpFlags&STIEFImpDelete) {
		rounds=DmNumRecordsInCategory(srcDB, dmAllCategories);
		SetupView(true, noListSelection);
	}
}

/**
 * ISEH
 */
Boolean
ISEH(EventType *ev)
{
	EntropyAdd(pool, ev);

	if (ev->eType==ctlSelectEvent) {
		switch (ev->data.ctlSelect.controlID) {
		case cImportSettingsImport:
			state.impExpFlags&=~(STIEFImpDelete|STIEFImpTemplate);
			if (CtlGetValue(UIObjectGet(cImportSettingsDelete)))
				state.impExpFlags|=STIEFImpDelete;
			if (CtlGetValue(UIObjectGet(cImportSettingsTemplate)))
				state.impExpFlags|=STIEFImpTemplate;
			break;
		case cImportSettingsPopup:
			CategorySelect(dstDB, currentForm, cImportSettingsPopup, cImportSettingsList, true, &state.impCat, catName, 1, 0);
			return true;
		case cImportSettingsIcon:
			fIconRun(&iconID);
			UIFormRedraw();
			return true;
		}
	} else if (ev->eType==frmUpdateEvent) {
		if (dbRefIcons) {
			RectangleType r;

			FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cImportSettingsIcon), &r);
			WinEraseRectangle(&r, 0);

			fMainDrawIcon(iconID, r.topLeft.x, r.topLeft.y);
			WinDrawRectangleFrame(popupFrame, &r);
		}
		return true;
	}

	return false;
}

/**
 * Show Export settings dialog and optionally export items.
 */
static void
Export(void)
{
	UInt16 expCnt=0, rIdx=0, dIdx, sSize, attrs, curIdx, idx, bit;
	Char *lf="\n", *NUL="\x00";
	Char buffer[34];
	RecordHdr *h;
	UInt32 size, tSize;
	FormSave frm;
	MemHandle mh, rh;
	Char *d, *s;

	UIFormPrologue(&frm, fExportSettings, ESEH);

	UIPopupSet(srcDB, state.expCat, catName, cExportSettingsPopup);

	if (state.impExpFlags&STIEFExpDelete)
		CtlSetValue(UIObjectGet(cExportSettingsDelete), 1);

	if (UIFormEpilogue(&frm, NULL, cExportSettingsCancel)==false)
		return;

	for (curIdx=0; curIdx<rounds; rIdx++) {
		DmRecordInfo(srcDB, rIdx, &attrs, NULL, NULL);
		if (attrs&dmRecAttrDelete)
			continue;

		idx=rIdx/8;
		bit=1<<(rIdx&7);
		if (bitMap[idx]&bit) {
			mh=DmQueryRecord(srcDB, rIdx);
			ErrFatalDisplayIf(mh==NULL, "(Export) Failed to get record from iSecur");

			h=MemHandleLock(mh);
			s=((Char *)h)+sizeof(RecordHdr);
			if (h->flags&RHHasTitle) {
				tSize=StrLen(((Char *)h)+sizeof(RecordHdr));
				s+=tSize+1;
				if (!(tSize&1))
					s++;
			} else
				tSize=0;


			if (h->flags&RHHasSecret)
				sSize=MemHandleSize(mh)-(s-(Char *)h);
			else
				sSize=0;

			if (tSize)
				size=tSize+1;
			else
				size=0;

			size+=(UInt32)sSize;
			if (size) {
				dIdx=dmMaxRecordIndex;
				rh=DmNewRecord(dstDB, &dIdx, size+1);
				ErrFatalDisplayIf(rh==NULL, "(Export) Out of memory");
				d=MemHandleLock(rh);
				bit=0;
				if (tSize) {
					DmWrite(d, bit, ((Char *)h)+sizeof(RecordHdr), tSize);
					bit+=tSize;
					DmWrite(d, bit, lf, 1);
					bit++;
				}

				if (sSize) {
					buffer[32]=0;
					for (idx=0; idx<sSize; idx+=32) {
						AESDecrypt(&s[idx], buffer);
						AESDecrypt(&s[idx+16], buffer+16);
						attrs=StrLen(buffer);
						DmWrite(d, bit, buffer, attrs);
						bit+=attrs;
					}
				}

				DmWrite(d, bit, NUL, 1);
				
				DmRecordInfo(dstDB, dIdx, &attrs, NULL, NULL);
				attrs&=~dmRecAttrCategoryMask;
				attrs|=state.expCat;
				DmSetRecordInfo(dstDB, dIdx, &attrs, NULL);
				DmReleaseRecord(dstDB, dIdx, true);

				MemHandleUnlock(rh);
			}

			MemHandleUnlock(mh);

			if (state.impExpFlags&STIEFExpDelete) 
				DmDeleteRecord(srcDB, rIdx);

			expCnt++;
		}
		curIdx++;
	}
	DmQuickSort(dbData.db, DBSort, 0);
	MemSet(bitMap, bitCnt, 0);
	StrPrintF(buffer, "%u", expCnt);
	FrmCustomAlert(aExpExported, buffer, NULL, NULL);

	if (state.impExpFlags&STIEFExpDelete) {
		rounds=DmNumRecordsInCategory(srcDB, dmAllCategories);
		SetupView(true, noListSelection);
	}
}

/**
 * ESEH
 */
Boolean
ESEH(EventType *ev)
{
	EntropyAdd(pool, ev);

	if (ev->eType==ctlSelectEvent) {
		switch (ev->data.ctlSelect.controlID) {
		case cExportSettingsExport:
			if (CtlGetValue(UIObjectGet(cExportSettingsDelete)))
				state.impExpFlags|=STIEFExpDelete;
			else
				state.impExpFlags&=~STIEFExpDelete;
			break;
		case cExportSettingsPopup:
			CategorySelect(dstDB, currentForm, cExportSettingsPopup, cExportSettingsList, true, &state.expCat, catName, 1, 0);
			return true;
		}
	}

	return false;
}

