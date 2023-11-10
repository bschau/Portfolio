/*
 * fEnd.c
 */
#include "Include.h"

/* globals */
static UInt16 qualifyPos, score;
static Char name[dlkUserNameBufSize];

/*
 * fEndInit
 */
Boolean
fEndInit(void)
{
	score=session->score;
	qualifyPos=LowQualify(score);
	MemSet(name, sizeof(name), 0);
	return true;
}

/*
 * fEndEH
 */
Boolean
fEndEH(EventType *ev)
{
	Char buffer[32];
	FontID fId;
	UInt16 pW, len;
	Boolean trunc;

	switch (ev->eType) {
	case frmOpenEvent:
		SssDestroy();

		FrmDrawForm(currentForm);
		if (qualifyPos) {
			StrPrintF(buffer, "%d.", qualifyPos);
			UIFieldSetText(cEndPosition, buffer);

			if (UtilsGetPref(name, dlkUserNameBufSize, PrfLastName)==false)
				DlkGetSyncInfo(NULL, NULL, NULL, name, NULL, NULL);

			if (name[0])
				UIFieldSetText(cEndName, name);

			UIShowObject(cEndEN1);
			UIShowObject(cEndEN2);
			UIShowObject(cEndPosition);
			UIShowObject(cEndName);
			UIFieldFocus(cEndName);
		}

	case frmUpdateEvent:	/* FALL-THRU */
		StrPrintF(buffer, "You Scored: %d", score);
		len=StrLen(buffer);
		pW=160;
		fId=FntSetFont(stdFont);
		FntCharsInWidth(buffer, &pW, &len, &trunc);
		WinDrawChars(buffer, len, (160-pW)/2, 64);
		FntSetFont(fId);
		return true;

	case frmCloseEvent:
		if (qualifyPos) {
			Char *n=UIFieldGetText(cEndName);

			if (StrLen(n)>0) {
				MemSet(name, sizeof(name), 0);
				StrNCopy(name, n, dlkUserNameBufSize);
			}

			UtilsSetPref(name, sizeof(name), PrfLastName);
			LowAddPair(score, name);
		}
		break;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cEndOK) {
			FrmGotoForm(fMain);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}
