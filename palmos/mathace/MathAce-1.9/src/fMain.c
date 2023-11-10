/*
 * fMain.c
 */
#include "Include.h"

/* protos */

/* globals */

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	inGame=false;
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	MemHandle mh;
	Boolean color;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		if (WinScreenMode(winScreenModeGetSupportsColor, NULL, NULL, NULL, &color)!=errNone)
			color=false;

		if ((mh=DmGetResource(iconType, 1000))) {
			WinPaintBitmap(MemHandleLock(mh), 64, 18);
			MemHandleUnlock(mh);
			DmReleaseResource(mh);
		}
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainPreferences:
			fPreferencesRun();
			return true;
		case cMainBeam:
			IRBeam();
			return true;
		case cMainScores:
			FrmGotoForm(fScores);
			return true;
		case cMainClear:
			if (FrmAlert(aClearScores)==0)
				MemSet(record, sizeof(Record)*MaxRecords, 0);

			return true;
		case cMainStart:
			SssNew();
			FrmGotoForm(fGame);
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainStart:
			SssNew();
			FrmGotoForm(fGame);
			return true;

		case mMainScores:
			FrmGotoForm(fScores);
			return true;

		case mMainPreferences:
			fPreferencesRun();
			return true;

		case mMainBeam:
			IRBeam();
			return true;

		case mMainAbout:
			About();
			return true;

		case mMainClear:
			if (FrmAlert(aClearScores)==0)
				MemSet(record, sizeof(Record)*MaxRecords, 0);

			return true;
		}

	default:	/* FALL-THRU */
		break;
	}

	return false;
}
