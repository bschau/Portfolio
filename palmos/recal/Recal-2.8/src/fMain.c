/*
 * fMain.c
 */
#include "Include.h"

/* globals */
static Boolean hasNav;

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	ListType *l=UIObjectGet(cMainACList);
	Char buffer[16];

	hasNav=PMNavSupported();

	StrPrintF(buffer, "%u", prefs.delay);
	UIFieldSetText(cMainDelay, buffer);
	StrPrintF(buffer, "%u", prefs.timeout);
	UIFieldSetText(cMainTimeout, buffer);
	UIFieldFocus(cMainDelay);

	if (NotificationsExists()) {
		UIObjectShow(cMainLabel);
		UIObjectShow(cMainHotSync);
		if (prefs.flags&PFlgHotSync)
			CtlSetValue(UIObjectGet(cMainHotSync), 1);

		if (PMOS5Device()) {
			UIObjectShow(cMainLaunching);
			if (prefs.flags&PFlgLaunching)
				CtlSetValue(UIObjectGet(cMainLaunching), 1);

			UIObjectShow(cMainQuitting);
			if (prefs.flags&PFlgQuitting)
				CtlSetValue(UIObjectGet(cMainQuitting), 1);

			UIObjectShow(cMainWakeup);
			if (prefs.flags&PFlgWakeup)
				CtlSetValue(UIObjectGet(cMainWakeup), 1);
		}
	}

	LstSetSelection(l, prefs.autoDigi);
	LstMakeItemVisible(l, prefs.autoDigi);
	CtlSetLabel(UIObjectGet(cMainACPopup), LstGetSelectionText(l, prefs.autoDigi));

	l=UIObjectGet(cMainPList);
	LstSetSelection(l, prefs.precision);
	LstMakeItemVisible(l, prefs.precision);
	CtlSetLabel(UIObjectGet(cMainPPopup), LstGetSelectionText(l, prefs.precision));
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case frmSaveEvent:
		fMainSavePrefs();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainRecalibrate:
			fMainSavePrefs();
			FrmGotoForm(fDigitize);
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			fAboutRun();
			FrmUpdateForm(fMain, frmRedrawUpdateCode);
			return true;
		}
		break;
	case keyDownEvent:
		if (!hasNav) {
			if (ev->data.keyDown.chr==vchrPageUp || ev->data.keyDown.chr==vchrPageDown) {
				fMainSavePrefs();
				fDigitizerRun(true, prefs.timeout, PMPrecision(prefs.precision));
				return true;
			}
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * fMainSavePrefs
 *
 * Save preferences.
 */
void
fMainSavePrefs(void)
{
	Char *p;
	Int16 sel;

	if ((p=UIFieldGetText(cMainDelay))) {
		Int16 d=StrAToI(p);

		if (d<0)
			d=0;

		prefs.delay=d;
	}

	if ((p=UIFieldGetText(cMainTimeout))) {
		Int16 d=StrAToI(p);

		if (d<10)
			d=10;

		prefs.timeout=d;
	}

	prefs.flags=0;
	if (NotificationsExists()) {
		if (CtlGetValue(UIObjectGet(cMainHotSync)))
			prefs.flags|=PFlgHotSync;

		if (PMOS5Device()) {
			if (CtlGetValue(UIObjectGet(cMainLaunching)))
				prefs.flags|=PFlgLaunching;

			if (CtlGetValue(UIObjectGet(cMainQuitting)))
				prefs.flags|=PFlgQuitting;

			if (CtlGetValue(UIObjectGet(cMainWakeup)))
				prefs.flags|=PFlgWakeup;
		}
	}

	if ((sel=LstGetSelection(UIObjectGet(cMainACList)))==noListSelection)
		prefs.autoDigi=0;
	else
		prefs.autoDigi=sel;

	if ((sel=LstGetSelection(UIObjectGet(cMainPList)))==noListSelection)
		prefs.precision=0;
	else
		prefs.precision=sel;

	PMSetPref(&prefs, sizeof(Prefs), PrfApplication);
	PMSetupAutoDigi();
}
