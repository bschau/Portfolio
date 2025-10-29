/*
 * fShow.c
 */
#include "Include.h"

/* protos */
_FRM(static void UpdateTitle(void));
_FRM(static void UpdateCategory(void));
_FRM(static void Beam(void));

/* globals */
static Char *tArrLeft="\x02", *tArrRight="\x03", *tArrLeftDis="\x05", *tArrRightDis="\x06";
extern DmResID iconID;
extern UInt8 *gTitle;
extern UInt16 gTitleIdx, catIdx;
extern Boolean editable, isTemplate;
extern UInt16 libRefNum;
extern MemHandle aesData;
extern UInt16 recordIdx;
extern DB dbData;
extern DmOpenRef dbRefIcons;

/*
 * fShowOpen
 */
void
fShowOpen(void)
{
	MemHandle mh=MemHandleNew(TitleLength+1);

	ErrFatalDisplayIf(mh==NULL, "(fShowOpen) Out of memory");
	gTitle=MemHandleLock(mh);
}

/*
 * fShowClose
 */
void
fShowClose(void)
{
	if (gTitle)
		MemPtrFree(gTitle);
}

/*
 * fShowRun
 *
 * Setup for viewing record.
 *
 *  -> rIdx		Record Index (or dmMaxRecordIndex for new record).
 *  -> hasEdit		Show Edit button.
 */
void
fShowRun(UInt16 rIdx, Boolean hasEdit)
{
	recordIdx=rIdx;
	editable=hasEdit;
	FrmGotoForm(fShow);
}

/*
 * fShowInit
 */
Boolean
fShowInit(void)
{
	MemHandle mh;
	RecordHdr *h;
	UInt8 *p;

	if (recordIdx==dmMaxRecordIndex) {
		if (editable)
			FrmGotoForm(fMain);
		else
			PMQueueAppStopEvent();

		return true;
	}

	DmRecordInfo(dbData.db, recordIdx, &catIdx, NULL, NULL);
	catIdx&=dmRecAttrCategoryMask;

	mh=DBGetRecord(&dbData, recordIdx);
	h=MemHandleLock(mh);
	p=((UInt8 *)h)+sizeof(RecordHdr);

	iconID=h->iconID;

	if (h->flags&RHTemplate)
		isTemplate=true;
	else
		isTemplate=false;

	gTitleIdx=0;
	if (h->flags&RHHasTitle) {
		UInt32 len=StrLen(p)+1;

		MemMove(gTitle, p, len);
		if (len&1)
			len++;
		p+=len;
	} else {
		MemSet(gTitle, TitleLength+1, 0);
		SysStringByIndex(strShowDialog, 0, gTitle, TitleLength);
	}

	if (h->flags&RHHasSecret) {
		UInt32 hLen=MemHandleSize(mh)-(p-(UInt8 *)h), idx;
		MemHandle rh;
		UInt8 *e;

		rh=MemHandleNew(hLen);
		ErrFatalDisplayIf(rh==NULL, "(fShowInit) Out of memory");
		e=MemHandleLock(rh);
		MemMove(e, p, hLen);

		for (idx=0; idx<hLen; idx+=32) {
			AESDecrypt(&e[idx], &e[idx]);
			AESDecrypt(&e[idx+16], &e[idx+16]);
		}

		UIFieldSetText(cShowFldSecret, e);
		FldSetMaxChars(UIObjectGet(cShowFldSecret), StrLen(e));
		MemHandleFree(rh);
		UIFieldUpdateScrollBar(cShowFldSecret, cShowScrBarSecret);
	}

	MemHandleUnlock(mh);

	if (editable)
		UIObjectShow(cShowEdit);

	CtlSetLabel(UIObjectGet(cShowToggle), "L");

	return true;
}

/*
 * fShowEH
 */
Boolean
fShowEH(EventType *ev)
{
	UInt16 tLen;

	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case frmOpenEvent:
	case frmUpdateEvent:
		FrmDrawForm(currentForm);
		UpdateTitle();
		UpdateCategory();
		return true;

	case frmSaveEvent:
		UIFieldClear(cShowFldSecret);
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cShowDone:
			if (editable)
				FrmGotoForm(fMain);
			else
				PMQueueAppStopEvent();

			return true;
		case cShowEdit:
			fEditRun(recordIdx, 0, false);
			return true;
		case cShowPrev:
			if (gTitleIdx<10)
				gTitleIdx=0;
			else
				gTitleIdx-=10;
			UpdateTitle();
			return true;
		case cShowNext:
			if ((tLen=StrLen(gTitle))>10)
				tLen-=10;

			if (gTitleIdx>tLen)
				gTitleIdx=tLen;
			else
				gTitleIdx+=10;

			UpdateTitle();
			return true;
		case cShowBeam:
			Beam();
			return true;
		case cShowToggle:
			{
			FieldType *fld=UIObjectGet(cShowFldSecret);
			ControlType *ctl=UIObjectGet(cShowToggle);
			FieldAttrType attr;
			const Char *p;

			p=CtlGetLabel(ctl);
			FldGetAttributes(fld, &attr);
			if (*p=='L') {
				CtlSetLabel(ctl, "U");
				attr.editable=1;
			} else {
				CtlSetLabel(ctl, "L");
				attr.editable=0;
				FldSetSelection(fld, 0, 0);
				FldReleaseFocus(fld);
			}
			FldSetAttributes(fld, &attr);

			}
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mShowCreate:
			fLetCreateRun(recordIdx);
			UIFormRedraw();
			return true;
		case mShowBeam:
			Beam();
			return true;
		case mShowEdit:
			fEditRun(recordIdx, 0, false);
			return true;
		default:
			break;
		}
		break;

	case keyDownEvent:
		if (UIFieldScrollBarKeyHandler(ev, cShowFldSecret, cShowScrBarSecret)==false)
			return UIFieldScrollBarHandler(ev, cShowFldSecret, cShowScrBarSecret);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, cShowFldSecret, cShowScrBarSecret);
		break;
	}

	return false;
}

/*
 * UpdateTitle
 *
 * Draw the icon, title and controls.
 */
static void
UpdateTitle(void)
{
	UInt16 tLen=StrLen(&gTitle[gTitleIdx]);
	ControlType *ctl;
	FontID fID;
	RectangleType r;
	Boolean fitAll;

	RctSetRectangle(&r, 0, 16, 160, 11);
	WinEraseRectangle(&r, 0);

	if (dbRefIcons && (iconID!=noIconSelected)) {
		UInt16 rIdx;

		if ((rIdx=DmFindResource(dbRefIcons, bitmapRsc, iconID, NULL))!=dmMaxRecordIndex) {
			MemHandle mh;
	
			if ((mh=DmGetResourceIndex(dbRefIcons, rIdx))) {
				WinDrawBitmap(MemHandleLock(mh), r.topLeft.x, r.topLeft.y+1);
				MemHandleUnlock(mh);
				DmReleaseResource(mh);

				r.topLeft.x+=12;
				r.extent.x-=12;
			}
		}
	}

	r.extent.x-=20;

	fID=FntSetFont(boldFont);
	FntCharsInWidth(&gTitle[gTitleIdx], &r.extent.x, &tLen, &fitAll);
	WinDrawChars(&gTitle[gTitleIdx], tLen, r.topLeft.x, r.topLeft.y);
	FntSetFont(fID);

	ctl=UIObjectGet(cShowPrev);
	if (gTitleIdx) {
		CtlSetEnabled(ctl, 1);
		CtlSetLabel(ctl, tArrLeft);
	} else {
		CtlSetEnabled(ctl, 0);
		CtlSetLabel(ctl, tArrLeftDis);
	}

	ctl=UIObjectGet(cShowNext);
	if (fitAll==false) {
		CtlSetEnabled(ctl, 1);
		CtlSetLabel(ctl, tArrRight);
	} else {
		CtlSetEnabled(ctl, 0);
		CtlSetLabel(ctl, tArrRightDis);
	}
}

/*
 * UpdateCategory
 *
 * Draw the cateory name in the upper rigth corner.
 */
static void
UpdateCategory(void)
{
	FontID fID=FntSetFont(stdFont);
	UInt16 pW=80, len;
	Char catName[dmCategoryLength];
	Boolean fitAll;
	RectangleType r;

	CategoryGetName(dbData.db, catIdx, catName);
	len=StrLen(catName);
	FntCharsInWidth(catName, &pW, &len, &fitAll);
	pW=FntCharsWidth(catName, len);
	RctSetRectangle(&r, 80, 1, 80, 11);
	WinEraseRectangle(&r, 0);
	WinDrawChars(catName, len, r.topLeft.x+r.extent.x-pW, r.topLeft.y);
	FntSetFont(fID);
}

/*
 * Beam
 *
 * Beam the entry.
 */
static void
Beam(void)
{
	UInt16 len=1+2+2;	/* template flag + 2 * sizes */
	UInt16 tLen=0, sLen=0;
	MemHandle mh;
	UInt8 *p;
	Char *c;
	ExgSocketType sck;
	Err err;
	Char buffer[20];

	tLen=StrLen(gTitle);
	c=UIFieldGetText(cShowFldSecret);
	if (c)
		sLen=StrLen(c);

	len+=tLen+sLen;

	mh=MemHandleNew(len);
	ErrFatalDisplayIf(mh==NULL, "(Beam) Out of memory");
	p=MemHandleLock(mh);

	if (isTemplate)
		*p++=1;
	else
		*p++=0;

	*p++=(tLen>>8)&0xff;
	*p++=tLen&0xff;
	if (tLen) {
		MemMove(p, gTitle, tLen);
		p+=tLen;
	}

	*p++=(sLen>>8)&0xff;
	*p++=sLen&0xff;
	if (sLen)
		MemMove(p, c, sLen);

	MemSet(&sck, sizeof(ExgSocketType), 0);
	StrPrintF(buffer, "iSecur %s", isTemplate ? "template" : "secret");

	sck.description=buffer;
	sck.target=(UInt32)CRID;
	sck.name="Entry";
	sck.type="iSec";
	sck.length=MemHandleSize(mh);
	if ((err=ExgPut(&sck))==errNone) {
		ExgSend(&sck, MemHandleLock(mh), sck.length, &err);
		MemHandleUnlock(mh);
		err=ExgDisconnect(&sck, err);
	}

	MemHandleFree(mh);

	if (err!=errNone)
		FrmCustomAlert(aIRSendEntryError, isTemplate ? "template" : "secret", NULL, NULL);
}
