/*
 * fDetails.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/* globals */
static UInt16 focusBand[]={ cDetailsApplicationName, cDetailsHotSyncName, 0 };
static HotSyncRecord *rec;
static UInt16 *ret;

/*
 * fDetailsGet
 *
 * Get HotSync and Application names from user.
 *
 * <-  rS		Return status.
 * <-> hsr		HotSyncRecord.
 */
void
fDetailsGet(UInt16 *rS, HotSyncRecord *hsr)
{
	FormSave frm;
	FieldType *fld;

	ret=rS;
	rec=hsr;
	UIFormPrologue(&frm, fDetails, EH);

	if (StrLen(hsr->appName)>0)
		UIFieldSetText(cDetailsApplicationName, hsr->appName);

	fld=UIObjectGet(cDetailsHotSyncName);
	if (StrLen(hsr->hotSyncName)>0) {
		UIFieldSetText(cDetailsHotSyncName, hsr->hotSyncName);
		FldSetSelection(fld, 0, FldGetTextLength(fld));
		UIObjectShow(cDetailsDelete);
	} else if (StrLen(prefs.lastHotSyncName)>0) {
		UIFieldSetText(cDetailsHotSyncName, prefs.lastHotSyncName);
		FldSetSelection(fld, 0, FldGetTextLength(fld));
	}

	UIFieldFocus(cDetailsHotSyncName);
	UIFormEpilogue(&frm, NULL, cDetailsCancel);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent) {
		Char *p;

		switch (ev->data.ctlSelect.controlID) {
		case cDetailsOK:
			MemSet(rec, sizeof(HotSyncRecord), 0);
			if ((p=UIFieldGetText(cDetailsApplicationName)))
				StrNCopy(rec->appName, p, NameLength);

			if ((p=UIFieldGetText(cDetailsHotSyncName))) {
				StrNCopy(rec->hotSyncName, p, HotSyncNameLength);
				StrNCopy(prefs.lastHotSyncName, p, HotSyncNameLength);
			} else 
				prefs.lastHotSyncName[0]='\x00';

			*ret=HsngOK;
			break;
		case cDetailsDelete:
			if (FrmAlert(aDelete)>0)
				return true;

			*ret=HsngDelete;
			break;
		default:
			*ret=HsngCancel;
			break;
		}
	} else if (ev->eType==keyDownEvent)
		UIFieldRefocus(focusBand, ev->data.keyDown.chr);

	return false;
}
