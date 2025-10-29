/*
 * fEdit.c
 */
#include "Include.h"

/* protos */
static void SaveData(void);
static void UpdateStatusBar(void);
static void CmdBack(void);
static void CmdSearch(void);
static Boolean CmdSearchEH(EventType *);
static void CmdDetails(void);
static Boolean CmdDetailsEH(EventType *);
static void CmdPlua(void);
static Boolean LaunchPlua(UInt32, UInt16);
static Boolean GadgetHandler(UInt16, UInt16);

/* globals */
static Char searchTerm[SearchTermLength+2];
static MemHandle recH;
static UInt32 uid;
static UInt16 catSave;
static FieldType *fldEdit;

/*
 * fEditRun
 *
 * Open the source for editing.
 *  
 *  -> rIdx		Source index.
 */
void
fEditRun(UInt16 rIdx)
{
	recordIdx=rIdx;
	DmRecordInfo(dbMemo.db, rIdx, NULL, &uid, NULL);
	FrmGotoForm(fEdit);
}

/*
 * fEditInit
 */
Boolean
fEditInit(void)
{
	FieldAttrType attrs;
	Char *p;
	UInt16 size;

	fldEdit=UIObjectGet(cEditField);

	if ((recH=DmGetRecord(dbMemo.db, recordIdx))==NULL) {
		DmReleaseRecord(dbMemo.db, recordIdx, true);
		recH=DmGetRecord(dbMemo.db, recordIdx);
	}
	ErrFatalDisplayIf(recH==NULL, "(fEditInit) Record not found.");

	p=MemHandleLock(recH);
	size=(UInt16)StrLen(p);
	FldSetText(fldEdit, recH, 0, size+1);
	FldSetMaxChars(fldEdit, prefs.sourceSize);
	FldRecalculateField(fldEdit, false);
	MemHandleUnlock(recH);

	if (prefs.flags&PFlgUnderlined) {
		FldGetAttributes(fldEdit, &attrs);
		attrs.underlined=grayUnderline;
		FldSetAttributes(fldEdit, &attrs);
	}

	if (uid==prefs.uid) {
		if (prefs.insPt>size)
			prefs.insPt=size;
	} else
		prefs.insPt=size;

	prefs.uid=uid;

	FldSetInsPtPosition(fldEdit, prefs.insPt);
	UIFieldFocus(cEditField);
	UIFieldUpdateScrollBar(cEditField, cEditScrollBar);
	return true;
}

/*
 * fEditEH
 */
Boolean
fEditEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateStatusBar();
		return true;

	case frmSaveEvent:
		SaveData();
		return true;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mEditBack:
			CmdBack();
			return true;
		case mEditSearch:
		   	CmdSearch();
			return true;
		case mEditDetails:
			CmdDetails();
			return true;
		case mEditPlua:
			CmdPlua();
			return true;
		}
		break;

	case penDownEvent:
		return GadgetHandler(ev->screenX, ev->screenY);

	case keyDownEvent:
		UpdateStatusBar();
		UIFieldScrollBarKeyHandler(ev, cEditField, cEditScrollBar);
	default:
		UIFieldScrollBarHandler(ev, cEditField, cEditScrollBar);
		break;
	}
	return false;
}

/*
 * SaveData
 *
 * Save this source.
 */
static void
SaveData(void)
{
	if (recordIdx!=dmMaxRecordIndex) {
		Char *p=UIFieldGetText(cEditField);
		Boolean dirty;

		if ((dirty=FldDirty(fldEdit)))
			FldCompactText(fldEdit);

		prefs.insPt=FldGetInsPtPosition(fldEdit);

		DmReleaseRecord(dbMemo.db, recordIdx, dirty);
		FldSetTextHandle(fldEdit, NULL);

		if (!p || (StrLen(p)==0))
			DmDeleteRecord(dbMemo.db, recordIdx);
	}
}

/*
 * UpdateStatusBar
 *
 * Update the status bar.
 */
static void
UpdateStatusBar(void)
{
	UInt16 pW=30, len;
	Char buffer[16];
	RectangleType r;
	Boolean trunc;

	StrPrintF(buffer, "%u", FldGetTextLength(fldEdit));
	len=StrLen(buffer);

	r.topLeft.x=110;
	r.topLeft.y=149;
	r.extent.x=30;
	r.extent.y=11;
	WinEraseRectangle(&r, 0);

	FntCharsInWidth(buffer, &pW, &len, &trunc);
	WinDrawChars(buffer, len, r.topLeft.x+30-pW, r.topLeft.y);
}

/*
 * CmdBack
 *
 * Back command.
 */
static void
CmdBack(void)
{
	SaveData();
	FrmGotoForm(fMain);
}

/*
 * CmdSearch
 *
 * Search command.
 */
static void
CmdSearch(void)
{
	UInt16 position=0, index;
	Char *src, *tmpSrc;
	FormSave frm;
	Boolean match;
	Char sT[SearchTermLength+2];

	UIFormPrologue(&frm, fEditSearch, CmdSearchEH);
	if (StrLen(searchTerm)>0) {
		FieldType *fld=UIObjectGet(cEditSearchTerm);

		UIFieldSetText(cEditSearchTerm, searchTerm);
		FldSetSelection(fld, 0, FldGetTextLength(fld));
	}

	UIFieldFocus(cEditSearchTerm);
	if (prefs.flags&PFlgSearchFromTop)
		CtlSetValue(UIObjectGet(cEditSearchFromTop), 1);

	if (UIFormEpilogue(&frm, NULL, cEditSearchCancel)==false)
		return;

	if (!*searchTerm)
		return;

	src=FldGetTextPtr(fldEdit);
	if (!(prefs.flags&PFlgSearchFromTop)) {
		UInt16 len=FldGetTextLength(fldEdit);
	       
		position=FldGetInsPtPosition(fldEdit);
		if (position>=len)
			return;

		src+=position;
	}

	for (index=0; searchTerm[index]; index++)
		sT[index]=searchTerm[index]&~32;

	sT[index]='\x00';

	for (; *src; ) {
		if ((*src&~32)==*sT) {
			index=1;
			tmpSrc=++src;
			position++;
			match=true;
			while (sT[index]) {
				if ((*tmpSrc&~32)==sT[index]) {
					tmpSrc++;
					index++;
				} else {
					match=false;
					break;
				}
			}

			if (match==true) {
				FldSetInsPtPosition(fldEdit, position-1);
				return;
			}
		} else {
			src++;
			position++;
		}
	}
}

/*
 * CmdSearchEH
 */
static Boolean
CmdSearchEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent && ev->data.ctlSelect.controlID==cEditSearchOK) {
		Char *p=UIFieldGetText(cEditSearchTerm);

		if ((CtlGetValue(UIObjectGet(cEditSearchFromTop))))
			prefs.flags|=PFlgSearchFromTop;
		else
			prefs.flags&=~PFlgSearchFromTop;

		if (p && StrLen(p)>0) {
			UInt16 len=StrLen(p);

			MemMove(searchTerm, p, len);
			searchTerm[len]='\x00';
		} else
			*searchTerm='\x00';
	}	

	return false;
}


/*
 * CmdDetails
 *
 * Details command.
 */
static void
CmdDetails(void)
{
	UInt16 lines=0, chars;
	FormSave frm;
	Char buffer[16];
	Char *p;

	catSave=prefs.category;

	/* NOTE. Field is forced to dirty. When the field is released the
	 * DmReleaseRecord will commit any changes done to the category. */
	FldSetDirty(fldEdit, true);
	chars=FldGetTextLength(fldEdit);
	for (p=FldGetTextPtr(fldEdit); *p; p++) {
		if (*p=='\n')
			lines++;
	}

	if (*(p-1)!='\n')
		lines++;
	
	UIFormPrologue(&frm, fEditDetails, CmdDetailsEH);
	StrPrintF(buffer, "%u", chars);
	UIFieldSetText(cEditDetailsCharacters, buffer);
	StrPrintF(buffer, "%u", lines);
	UIFieldSetText(cEditDetailsLines, buffer);
	DmRecordInfo(dbMemo.db, recordIdx, &chars, NULL, NULL);
	UIPopupSet(dbMemo.db, (chars&dmRecAttrCategoryMask), catName, cEditDetailsPopup);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * CmdDetailsEH
 */
static Boolean
CmdDetailsEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent) {
		UInt16 attrs;

		switch (ev->data.ctlSelect.controlID) {
		case cEditDetailsOK:
			DmRecordInfo(dbMemo.db, recordIdx, &attrs, NULL, NULL);
			attrs&=~dmRecAttrCategoryMask;
			attrs&=~dmSysOnlyRecAttrs;
			attrs|=(prefs.category|dmRecAttrDirty);
			DmSetRecordInfo(dbMemo.db, recordIdx, &attrs, NULL);
			break;
		case cEditDetailsCancel:
			prefs.category=catSave;
			break;
		case cEditDetailsDelete:
			prefs.category=catSave;
			if (FrmAlert(aDeleteSource)==0) {
				DmReleaseRecord(dbMemo.db, recordIdx, false);
				FldSetTextHandle(fldEdit, NULL);

				DmDeleteRecord(dbMemo.db, recordIdx);
				recordIdx=dmMaxRecordIndex;
				FrmGotoForm(fMain);
			} else
				return true;
			break;
		case cEditDetailsPopup:
			CategorySelect(dbMemo.db, currentForm, cEditDetailsPopup, cEditDetailsList, true, &prefs.category, catName, 1, 0);
			return true;
		}
	}

	return false;
}

/*
 * CmdPlua
 *
 * Switch to Plua command.
 */
static void
CmdPlua(void)
{
	SaveData();
	recordIdx=dmMaxRecordIndex;

	switch (prefs.pluaVersion) {
	case 0:
		if (LaunchPlua((UInt32)'LuaP', aCannotLaunchPlua2)==false) {
			if (LaunchPlua((UInt32)'Plua', aCannotLaunchPlua1)==false)
				FrmAlert(aNoPluas);
		}
		break;

	case 1:
		if (LaunchPlua((UInt32)'Plua', aCannotLaunchPlua1)==false) {
			if (LaunchPlua((UInt32)'LuaP', aCannotLaunchPlua2)==false)
				FrmAlert(aNoPluas);
		}
		break;

	case 2:
		if (LaunchPlua((UInt32)'LuaP', aCannotLaunchPlua2)==false)
			FrmAlert(aNoPluas);
		break;

	default:
		if (LaunchPlua((UInt32)'Plua', aCannotLaunchPlua1)==false)
			FrmAlert(aNoPluas);
		break;
	}
}

/*
 * LaunchPlua
 *
 * Launch the Plua given by crid.
 *
 *  -> crid		Plua to launch.
 *  -> alertId		Alert ID to show when Plua cannot be launched.
 *
 * Returns true if Plua was attempted launched.
 */
static Boolean
LaunchPlua(UInt32 crid, UInt16 alertId)
{
	UInt16 pCard;
	LocalID pID;
	DmSearchStateType state;

	if (DmGetNextDatabaseByTypeCreator(true, &state, sysFileTApplication, crid, true, &pCard, &pID)==errNone) {
		if (SysUIAppSwitch(pCard, pID, sysAppLaunchCmdNormalLaunch, NULL)!=errNone)
			FrmAlert(alertId);

		return true;
	}

	return false;
}

/*
 * GadgetHandler
 *
 * Handle a gadget tap.
 *
 *  -> x		Point X.
 *  -> y		Point y.
 *
 * Returns true if a gadget was pressed (and handled), false otherwise.
 */
static Boolean
GadgetHandler(UInt16 x, UInt16 y)
{
	UInt16 gdg[]={ cEditBack, cEditSearch, cEditDetails, cEditPlua, 0 };
	UInt16 bmps[]={ bmpBack, bmpSearch, bmpDetails, bmpPlua, 0 };
	UInt16 idx=0;
	Boolean wasInBounds, nowInBounds, penDown;
	RectangleType r;
	MemHandle mh;
	BitmapType *bmp;

	while (gdg[idx]) {
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, gdg[idx]), &r);
		if (RctPtInRectangle(x, y, &r)) {
			wasInBounds=true;
			WinInvertRectangle(&r, 0);
			do {
				PenGetPoint(&x, &y, &penDown);
				nowInBounds=RctPtInRectangle(x, y, &r);
				if (nowInBounds!=wasInBounds) {
					WinInvertRectangle(&r, 0);
					wasInBounds=nowInBounds;
				}
			} while (penDown);

			mh=DmGetResource(bitmapRsc, bmps[idx]);
			bmp=MemHandleLock(mh);
			WinDrawBitmap(bmp, r.topLeft.x, r.topLeft.y);
			MemHandleUnlock(mh);
			DmReleaseResource(mh);

			switch (gdg[idx]) {
			case cEditBack:
				CmdBack();
				break;
			case cEditSearch:
				CmdSearch();
				break;
			case cEditDetails:
				CmdDetails();
				break;
			case cEditPlua:
				CmdPlua();
				break;
			}

			return true;
		}

		idx++;
	}

	return false;
}
