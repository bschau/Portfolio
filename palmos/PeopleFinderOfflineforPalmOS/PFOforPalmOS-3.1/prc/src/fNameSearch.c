/*
 * fNameSearch.c
 */
#include "Include.h"

/* protos */
static void DrawList(Int16, RectangleType *, Char **);
static void NameChange(void);
static void UpdateSurnameControls(void);
static void UpdateSurname(void);
static void UpdateNameList(void);

/* globals */
static Char *buttons[]={ "\x02", "\x03", "\x05", "\x06" };
static UInt32 rowIds[NameSearchLength]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static Char phone[MaxPhoneLength];

/*
 * DrawList
 */
static void
DrawList(Int16 itemNum, RectangleType *rct, Char **unused)
{
	UInt32 idx=0, size;
	Int16 rIdx=itemNum;
	UInt16 pw=rct->extent.x, len;
	MemHandle mh;
	Char *p;
	Boolean trunc;

	WinEraseRectangle(rct, 0);
	mh=DBGetRecord(&dbNameSearch, recordIdx);
	p=MemHandleLock(mh);
	size=MemHandleSize(mh);

	for (EVER) {
		if ((idx=NSFindPerson(p, idx, size))==0) {
			MemHandleUnlock(mh);
			return;
		}

		if (!rIdx)
			break;

		rIdx--;

		idx=NSNextPerson(p, idx);
	}

	rowIds[itemNum-LstGetTopItem(UIObjectGet(cNameSearchList))]=idx;

	p+=idx+2;
	len=(UInt16)*p++;

	FntCharsInWidth(p, &pw, &len, &trunc);
	WinDrawChars(p, len, rct->topLeft.x, rct->topLeft.y);

	p+=len;
	if ((len=(UInt16)*p)==0) {
		p++;
		if ((len=(UInt16)*p)==0)
			p=NULL;
		else
			p=NSUncrunchPhone(phone, p);
	} else
		p=NSUncrunchPhone(phone, p);

	if (p) {
		UInt16 rLen;
		
		pw=120;
		rLen=StrLen(p);
		len=rLen;
		FntCharsInWidth(p, &pw, &len, &trunc);
		if (rLen>len) {
			*(p+len-1)='.';
			*(p+len-2)='.';
			*(p+len-3)='.';
			pw=120;
			FntCharsInWidth(p, &pw, &len, &trunc);
		}
		WinDrawChars(p, len, rct->topLeft.x+rct->extent.x-pw, rct->topLeft.y);
	}

	MemHandleFree(mh);
}

/*
 * fNameSearchInit
 */
Boolean
fNameSearchInit(void)
{
	UIListInit(cNameSearchList, DrawList, 0, noListSelection);
	return true;
}

/*
 * fNameSearchEH
 */
Boolean
fNameSearchEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		NameChange();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cNameSearchBack:
			FrmGotoForm(fMain);
			return true;
		case cNameSearchBSurname:
			recordIdx--;
			NameChange();
			return true;
		case cNameSearchFSurname:
			recordIdx++;
			NameChange();
			return true;
		}
		break;

	case lstSelectEvent:
		if (ev->data.lstSelect.selection!=noListSelection) {
			MemHandle mh;
			UInt32 idx;

			idx=rowIds[ev->data.lstSelect.selection-LstGetTopItem(UIObjectGet(cNameSearchList))];
			mh=DBGetRecord(&dbNameSearch, recordIdx);

			fDetailsRun(mh, idx);
			MemHandleFree(mh);
			return true;
		}
		break;

	case keyDownEvent:
		return UIListKeyHandler(ev, cNameSearchList);

	default:
		break;
	}

	return false;
}

/*
 * NameChange
 *
 * Stuff to do when Surname changes.
 */
static void
NameChange(void)
{
	UpdateSurnameControls();
	UpdateSurname();
	UpdateNameList();
}

/*
 * UpdateSurnameControls
 *
 * Redraw the Surname controls in full or ghosted mode.
 */
static void
UpdateSurnameControls(void)
{
	ControlType *ctl;

	ctl=UIObjectGet(cNameSearchBSurname);
	if (NSContainsPerson(-1)==true) {
		CtlSetLabel(ctl, buttons[0]);
		CtlSetEnabled(ctl, true);
	} else {
		CtlSetLabel(ctl, buttons[2]);
		CtlSetEnabled(ctl, false);
	}

	ctl=UIObjectGet(cNameSearchFSurname);
	if (NSContainsPerson(1)==true) {
		CtlSetLabel(ctl, buttons[1]);
		CtlSetEnabled(ctl, true);
	} else {
		CtlSetLabel(ctl, buttons[3]);
		CtlSetEnabled(ctl, false);
	}
}

/*
 * UpdateSurname
 *
 * Set the Surname in the surname field.
 */
static void
UpdateSurname(void)
{
	FontID f=FntSetFont(boldFont);
	UInt16 pw=125, len;
	MemHandle mh;
	Char *p;
	Boolean trunc;
	RectangleType r;

	mh=DBGetRecord(&dbNameSearch, recordIdx);

	p=NSUppercaseWords(MemHandleLock(mh));
	len=StrLen(p);
	FntCharsInWidth(p, &pw, &len, &trunc);

	RctSetRectangle(&r, 25, 18, 135, 12);
	WinEraseRectangle(&r, 0);
	WinDrawChars(p, len, r.topLeft.x, r.topLeft.y);
	
	MemPtrFree(p);
	MemHandleFree(mh);

	FntSetFont(f);
}

/*
 * UpdateNameList
 *
 * Update the list of names.
 */
static void
UpdateNameList(void)
{
	ListType *lst=UIObjectGet(cNameSearchList);

	UIListInit(cNameSearchList, DrawList, NSCountNames(), LstGetSelection(lst));
	LstDrawList(lst);
}
