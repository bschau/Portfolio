/*
 * fNew.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/* globals */
static UInt32 uid;

/*
 * fNewRun
 *
 * Create a new source file.
 */
UInt32
fNewRun(void)
{
	FormSave frm;

	uid=0;
	UIFormPrologue(&frm, fNew, EH);
	UIFieldFocus(cNewField);
	if (UIFormEpilogue(&frm, NULL, cNewCancel)==false)
		return 0;

	return uid;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	Char srcName[SourceNameLength+8];
	Char *src, *dst;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cNewOK:
			if ((src=UIFieldGetText(cNewField))==NULL)
				break;

			StrPrintF(srcName, "-- %s.lua\n", src);
			dst=srcName;
			for (src=srcName; *src; src++) {
				if (*src<' ')
					continue;

				*dst++=*src;
			}
			*dst++='\n';
			*dst='\x00';

			uid=DBSetRecord(&dbMemo, dmMaxRecordIndex, prefs.category, srcName, StrLen(srcName)+1, NULL);
			break;
		}
	default:
		break;
	}

	return false;
}
