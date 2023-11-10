/*
 * fNew.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/* globals */
static Int16 pluaVersion;

/*
 * fPreferencesRun
 */
void
fPreferencesRun(void)
{
	ListType *l;
	FormSave frm;
	Char buffer[16];

	UIFormPrologue(&frm, fPreferences, EH);
	StrPrintF(buffer, "%u", prefs.sourceSize);
	UIFieldSetText(cPreferencesSourceSize, buffer);
	UIFieldFocus(cPreferencesSourceSize);

	if (prefs.flags&PFlgUnderlined)
		CtlSetValue(UIObjectGet(cPreferencesUnderline), 1);

	pluaVersion=prefs.pluaVersion;
	l=UIObjectGet(cPreferencesList);
	LstSetSelection(l, pluaVersion);
	CtlSetLabel(UIObjectGet(cPreferencesPopup), LstGetSelectionText(l, pluaVersion));

	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	ListType *l;
	Char *p;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPreferencesOK:
			if ((p=UIFieldGetText(cPreferencesSourceSize))==NULL)
				break;

			if ((prefs.sourceSize=StrAToI(p))<1)
				prefs.sourceSize=DefaultSourceSize;

			if ((CtlGetValue(UIObjectGet(cPreferencesUnderline))))
				prefs.flags|=PFlgUnderlined;
			else
				prefs.flags&=~PFlgUnderlined;

			l=UIObjectGet(cPreferencesList);
			if ((prefs.pluaVersion=LstGetSelection(l))==noListSelection)
				pluaVersion=0;

			break;
		}
	default:
		break;
	}

	return false;
}
