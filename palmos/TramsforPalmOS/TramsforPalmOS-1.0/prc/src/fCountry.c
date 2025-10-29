/*
 * fCountry.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);
static void UpdatePopup(UInt16, UInt16);

/* globals */
static Int16 *co, *cu;

/*
 * fCountryRun
 *
 * Show the select country/currency dialog.
 *
 * <-> country		Country index.
 * <-> currency		Currency index.
 *
 * Returns true if OK was selected (country/currency will be updated), false
 * if cancel was pressed.
 */
Boolean
fCountryRun(Int16 *country, Int16 *currency)
{
	FormSave frm;

	co=country;
	cu=currency;
	UIFormPrologue(&frm, fCountry, EH);

	LocSetupList(cCountryCountryList, *co, false);
	UpdatePopup(cCountryCountryPopup, cCountryCountryList);
	LocSetupList(cCountryCurrencyList, *cu, true);
	UpdatePopup(cCountryCurrencyPopup, cCountryCurrencyList);

	return UIFormEpilogue(&frm, NULL, cCountryCancel);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	switch (ev->eType) {
	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cCountryOK) {
			if ((*co=LstGetSelection(UIObjectGet(cCountryCountryList)))==noListSelection)
				*co=0;

			if ((*cu=LstGetSelection(UIObjectGet(cCountryCurrencyList)))==noListSelection)
				*cu=0;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.listID==cCountryCountryList) {
			Int16 sel=ev->data.popSelect.selection;
			ListType *lst=UIObjectGet(cCountryCurrencyList);

			if (sel==noListSelection)
				break;

			sel=LocCountryToCurrency(sel);
			LstSetSelection(lst, sel);
			LstMakeItemVisible(lst, sel);
			UpdatePopup(cCountryCurrencyPopup, cCountryCurrencyList);
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdatePopup
 *
 * Update a popup with the text from the list.
 *
 *  -> pId		Popup ID.
 *  -> lId		List ID.
 */
static void
UpdatePopup(UInt16 pId, UInt16 lId)
{
	ListType *lst=UIObjectGet(lId);
	Int16 selection;
	Char *t="---";

	if ((selection=LstGetSelection(lst))!=noListSelection)
		t=LstGetSelectionText(lst, selection);

	CtlSetLabel(UIObjectGet(pId), t);
}
