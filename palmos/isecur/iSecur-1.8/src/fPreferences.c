/*
 * fPreferences.c
 */
#include "Include.h"

/* protos */
_UTL(static Boolean EH(EventType *));
_UTL(static void ShowPage(void));
_UTL(static void SaveData(void));
_UTL(static void SetCopyPrevention(void));

/* globals */
static UInt16 fPreferencesBandPage1[]={ cPreferencesFldRemember, cPreferencesFldRemind, 0 };
static UInt16 objs[]={ cPreferencesChkRemember, cPreferencesFldRemember, cPreferencesLblRemember, cPreferencesChkRemind, cPreferencesFldRemind, cPreferencesLblRemind1, cPreferencesLblRemind2, cPreferencesChkMax, cPreferencesFldMax, cPreferencesChkDestroyDB, cPreferencesLblDestroyDB, cPreferencesChkLogOut, cPreferencesChkTimeOut, cPreferencesChkAutoPopup, cPreferencesChkOpenEdit, cPreferencesChkFind, cPreferencesChkClipboard, cPreferencesChkNoBeam, cPreferencesChkNewSecret, cPreferencesChkSecretStay, cPreferencesChkTemplateInherit, cPreferencesChk5Way, 0 };
extern Prefs gPrefs, prefs;
extern State state;
extern Boolean notificationsEnabled;
extern UI *ui;
extern DB dbData;

/*
 * fPreferencesRun
 *
 * Show the Preferences form.
 */
void
fPreferencesRun(void)
{
	FormSave frm;
	ListType *lst;

	MemMove(&gPrefs, &prefs, sizeof(Prefs));
	UIFormPrologue(&frm, fPreferences, EH);

	lst=UIObjectGet(cPreferencesList);
	LstSetSelection(lst, state.prfPage);
	CtlSetLabel(UIObjectGet(cPreferencesPopup), LstGetSelectionText(lst, state.prfPage));

	ShowPage();
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPreferencesChkRemember:
			if (CtlGetValue(UIObjectGet(cPreferencesChkRemember)))
				UIFieldFocus(cPreferencesFldRemember);
			return true;
		case cPreferencesChkRemind:
			if (CtlGetValue(UIObjectGet(cPreferencesChkRemind)))
				UIFieldFocus(cPreferencesFldRemind);
			return true;
		case cPreferencesChkMax:
			if (CtlGetValue(UIObjectGet(cPreferencesChkMax)))
				UIFieldFocus(cPreferencesFldMax);
			return true;
		case cPreferencesOK:
			SaveData();
			MemMove(&prefs, &gPrefs, sizeof(Prefs));
			if (gPrefs.pwdRetention!=prefs.pwdRetention)
				PMSetPref(NULL, 0, PrfRetentionList, false);

			if (!prefs.pwdChangeDays)
				PMSetPref(NULL, 0, PrfPwdChangeAlarm, false);

			PMSetPref(&prefs, sizeof(Prefs), PrfApplication, true);
			PMPwdChgAlarm();
			SetCopyPrevention();
			break;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.listID==cPreferencesList) {
			if ((ev->data.popSelect.selection!=ev->data.popSelect.priorSelection) && (ev->data.popSelect.selection!=noListSelection)) {
				SaveData();
				state.prfPage=ev->data.popSelect.selection;
				ShowPage();
			}
		}
		break;

	case keyDownEvent:
		if (!state.prfPage)
			UIFieldRefocus(fPreferencesBandPage1, ev->data.keyDown.chr);

	default:	/* FALL-THRU */
		break;
	}
	
	return false;
}

/*
 * ShowPage
 *
 * Show controls on page #.
 */
static void
ShowPage(void)
{
	RectangleType r;
	UInt16 idx;
	Char buffer[16];

	for (idx=0; objs[idx]; idx++)
		UIObjectHide(objs[idx]);

	RctSetRectangle(&r, 0, 18, 160, 120);
	WinEraseRectangle(&r, 0);

	switch (state.prfPage) {
	case 0:
		UIObjectShow(cPreferencesChkRemember);
		UIObjectShow(cPreferencesFldRemember);
		UIObjectShow(cPreferencesLblRemember);
		if (gPrefs.pwdRetention) {
			CtlSetValue(UIObjectGet(cPreferencesChkRemember), 1);
			StrPrintF(buffer, "%u", gPrefs.pwdRetention);
			UIFieldSetText(cPreferencesFldRemember, buffer);
			UIFieldFocus(cPreferencesFldRemember);
		} else
			UIFieldClear(cPreferencesFldRemember);

		UIObjectShow(cPreferencesChkRemind);
		UIObjectShow(cPreferencesFldRemind);
		UIObjectShow(cPreferencesLblRemind1);
		UIObjectShow(cPreferencesLblRemind2);
		if (gPrefs.pwdChangeDays) {
			CtlSetValue(UIObjectGet(cPreferencesChkRemind), 1);
			StrPrintF(buffer, "%u", gPrefs.pwdChangeDays);
			UIFieldSetText(cPreferencesFldRemind, buffer);
			UIFieldFocus(cPreferencesFldRemind);
		} else
			UIFieldClear(cPreferencesFldRemind);

		break;

	case 1:
		UIObjectShow(cPreferencesChkMax);
		UIObjectShow(cPreferencesFldMax);
		if (gPrefs.maxFailedLogins) {
			CtlSetValue(UIObjectGet(cPreferencesChkMax), 1);
			StrPrintF(buffer, "%u", gPrefs.maxFailedLogins);
			UIFieldSetText(cPreferencesFldMax, buffer);
		} else
			UIFieldClear(cPreferencesFldMax);

		UIFieldFocus(cPreferencesFldMax);

		UIObjectShow(cPreferencesChkDestroyDB);
		UIObjectShow(cPreferencesLblDestroyDB);
		if (gPrefs.flags&PFlgDestroyDB)
			CtlSetValue(UIObjectGet(cPreferencesChkDestroyDB), 1);

		UIObjectShow(cPreferencesChkLogOut);
		if (gPrefs.flags&PFlgLogoutPower)
			CtlSetValue(UIObjectGet(cPreferencesChkLogOut), 1);

		if (notificationsEnabled) {
			UIObjectShow(cPreferencesChkTimeOut);
			if (gPrefs.flags&PFlgLogoutTimeout)
				CtlSetValue(UIObjectGet(cPreferencesChkTimeOut), 1);
		}

		UIObjectShow(cPreferencesChkAutoPopup);
		if (gPrefs.flags&PFlgAutoPopupKbd)
			CtlSetValue(UIObjectGet(cPreferencesChkAutoPopup), 1);
		break;

	case 2:
		UIObjectShow(cPreferencesChkOpenEdit);
		if (gPrefs.flags&PFlgSecretOpenEdit)
			CtlSetValue(UIObjectGet(cPreferencesChkOpenEdit), 1);

		UIObjectShow(cPreferencesChkFind);
		if (gPrefs.flags&PFlgEnableFind)
			CtlSetValue(UIObjectGet(cPreferencesChkFind), 1);

		UIObjectShow(cPreferencesChkClipboard);
		if (gPrefs.flags&PFlgEmptyClipboard)
			CtlSetValue(UIObjectGet(cPreferencesChkClipboard), 1);

		UIObjectShow(cPreferencesChkNoBeam);
		if (gPrefs.flags&PFlgNoBeam)
			CtlSetValue(UIObjectGet(cPreferencesChkNoBeam), 1);

		UIObjectShow(cPreferencesChkNewSecret);
		if (gPrefs.flags&PFlgNewSecret)
			CtlSetValue(UIObjectGet(cPreferencesChkNewSecret), 1);

		UIObjectShow(cPreferencesChkSecretStay);
		if (gPrefs.flags&PFlgSecretStay)
			CtlSetValue(UIObjectGet(cPreferencesChkSecretStay), 1);

		UIObjectShow(cPreferencesChkTemplateInherit);
		if (gPrefs.flags&PFlgTemplateInherit)
			CtlSetValue(UIObjectGet(cPreferencesChkTemplateInherit), 1);

		UIObjectShow(cPreferencesChk5Way);
		if (gPrefs.flags&PFlg5Way)
			CtlSetValue(UIObjectGet(cPreferencesChk5Way), 1);

	default:	/* FALL-THRU */
		break;
	}
}

/*
 * SaveData
 *
 * Save data from current page.
 */
static void
SaveData(void)
{
	Char *p;

	switch (state.prfPage) {
	case 0:
		gPrefs.pwdRetention=0;
		if (CtlGetValue(UIObjectGet(cPreferencesChkRemember))) {
			if ((p=UIFieldGetText(cPreferencesFldRemember))!=NULL)
				gPrefs.pwdRetention=(UInt16)StrAToI(p);
		}

		gPrefs.pwdChangeDays=0;
		if (CtlGetValue(UIObjectGet(cPreferencesChkRemind))) {
			if ((p=UIFieldGetText(cPreferencesFldRemind))!=NULL)
				gPrefs.pwdChangeDays=(UInt16)StrAToI(p);
		}
		break;

	case 1:
		gPrefs.maxFailedLogins=0;
		if (CtlGetValue(UIObjectGet(cPreferencesChkMax))) {
			if ((p=UIFieldGetText(cPreferencesFldMax))!=NULL)
				gPrefs.maxFailedLogins=(UInt16)StrAToI(p);
		}

		gPrefs.flags&=~(PFlgDestroyDB|PFlgLogoutPower|PFlgLogoutTimeout|PFlgAutoPopupKbd);
		if (CtlGetValue(UIObjectGet(cPreferencesChkDestroyDB)))
			gPrefs.flags|=PFlgDestroyDB;

		if (CtlGetValue(UIObjectGet(cPreferencesChkLogOut)))
			gPrefs.flags|=PFlgLogoutPower;

		if (notificationsEnabled) {
			if (CtlGetValue(UIObjectGet(cPreferencesChkTimeOut)))
				gPrefs.flags|=PFlgLogoutTimeout;
		}

		if (CtlGetValue(UIObjectGet(cPreferencesChkAutoPopup)))
			gPrefs.flags|=PFlgAutoPopupKbd;
		break;

	case 2:
		gPrefs.flags&=~(PFlgSecretOpenEdit|PFlgEnableFind|PFlgEmptyClipboard|PFlgNoBeam|PFlgNewSecret|PFlgSecretStay|PFlgTemplateInherit|PFlg5Way);
		if (CtlGetValue(UIObjectGet(cPreferencesChkOpenEdit)))
			gPrefs.flags|=PFlgSecretOpenEdit;

		if (CtlGetValue(UIObjectGet(cPreferencesChkFind)))
			gPrefs.flags|=PFlgEnableFind;

		if (CtlGetValue(UIObjectGet(cPreferencesChkClipboard)))
			gPrefs.flags|=PFlgEmptyClipboard;

		if (CtlGetValue(UIObjectGet(cPreferencesChkNoBeam)))
			gPrefs.flags|=PFlgNoBeam;

		if (CtlGetValue(UIObjectGet(cPreferencesChkNewSecret)))
			gPrefs.flags|=PFlgNewSecret;

		if (CtlGetValue(UIObjectGet(cPreferencesChkSecretStay)))
			gPrefs.flags|=PFlgSecretStay;

		if (CtlGetValue(UIObjectGet(cPreferencesChkTemplateInherit)))
			gPrefs.flags|=PFlgTemplateInherit;

		if (CtlGetValue(UIObjectGet(cPreferencesChk5Way)))
			gPrefs.flags|=PFlg5Way;

	default:	/* FALL-THRU */
		break;
	}
}

/*
 * SetCopyPrevention
 *
 * Set or unset Copy Prevention based on preferences.
 */
static void
SetCopyPrevention(void)
{
	UInt16 attr;

	if (DmDatabaseInfo(dbData.card, dbData.id, NULL, &attr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)==errNone) {
		if (prefs.flags&PFlgNoBeam)
			attr|=dmHdrAttrCopyPrevention;
		else
			attr&=~dmHdrAttrCopyPrevention;

		if (DmSetDatabaseInfo(dbData.card, dbData.id, NULL, &attr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)==errNone)
			return;
	}

	FrmAlert(aNoCopyPrevention);
}
