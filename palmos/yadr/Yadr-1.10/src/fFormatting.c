/*
 * fFormatting.c
 */
#include "Include.h"

/* protos */
static void GdgUpdate(void);

/* globals */
static UInt16 zeroPush[]={ cFormattingFontPush1, cFormattingFontPush2, cFormattingFontPush3, cFormattingAlignL, cFormattingAlignC, cFormattingAlignR, cFormattingAlignJ, cFormattingUpDown, cFormattingLeftRight, 0 };
static IndexedColorType f, b;
static UInt16 lineSpacing;

/*
 * fFormattingInit
 */
Boolean
fFormattingInit(void)
{
	ListType *lst;
	UInt16 idx;

	f=prefs.foreColor;
	b=prefs.backColor;
	lineSpacing=prefs.lineSpacing;
	for (idx=0; zeroPush[idx]; idx++)
		CtlSetValue(UIObjectGet(zeroPush[idx]), 0);

	CtlSetValue(UIObjectGet(cFormattingFontPush1+prefs.font), 1);
	CtlSetValue(UIObjectGet(cFormattingAlignL+prefs.drawMode), 1);
	CtlSetValue(UIObjectGet(cFormattingUpDown+prefs.controlMode), 1);

	lst=UIObjectGet(cFormattingSpacingList);
	LstSetSelection(lst, lineSpacing);
	CtlSetLabel(UIObjectGet(cFormattingSpacingPopup), LstGetSelectionText(lst, lineSpacing));

	lst=UIObjectGet(cFormattingViewList);
	idx=(prefs.flags&PFFullScreen) ? 1 : 0;
	LstSetSelection(lst, idx);
	CtlSetLabel(UIObjectGet(cFormattingViewPopup), LstGetSelectionText(lst, idx));
	return true;
}

/*
 * fFormattingEH
 */
Boolean
fFormattingEH(EventType *ev)
{
	RectangleType r;
	Int16 sel;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		GdgUpdate();
		return true;

	case penDownEvent:
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cFormattingGDGF), &r);
		if (RctPtInRectangle(ev->screenX, ev->screenY, &r)) {
			UIPickColor(&f, NULL, UIPickColorStartPalette, "Select Foreground Color", NULL);
			FrmUpdateForm(fFormatting, frmRedrawUpdateCode);
			return true;
		}

		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cFormattingGDGB), &r);
		if (RctPtInRectangle(ev->screenX, ev->screenY, &r)) {
			UIPickColor(&b, NULL, UIPickColorStartPalette, "Select Background Color", NULL);
			FrmUpdateForm(fFormatting, frmRedrawUpdateCode);
			return true;
		}
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cFormattingOK:
			prefs.foreColor=f;
			prefs.backColor=b;

			if (CtlGetValue(UIObjectGet(cFormattingFontPush1)))
				prefs.font=0;
			else if (CtlGetValue(UIObjectGet(cFormattingFontPush2)))
				prefs.font=1;
			else
				prefs.font=2;

			if (CtlGetValue(UIObjectGet(cFormattingAlignL)))
				prefs.drawMode=PDMLeft;
			else if (CtlGetValue(UIObjectGet(cFormattingAlignC)))
				prefs.drawMode=PDMCenter;
			else if (CtlGetValue(UIObjectGet(cFormattingAlignR)))
				prefs.drawMode=PDMRight;
			else
				prefs.drawMode=PDMJustify;

			if (CtlGetValue(UIObjectGet(cFormattingUpDown)))
				prefs.controlMode=PCMUpDown;
			else
				prefs.controlMode=PCMLeftRight;

			if ((sel=LstGetSelection(UIObjectGet(cFormattingSpacingList)))==noListSelection)
				prefs.lineSpacing=0;
			else
				prefs.lineSpacing=sel;

			prefs.flags&=~PFFullScreen;
			if (LstGetSelection(UIObjectGet(cFormattingViewList))==1)
				prefs.flags|=PFFullScreen;
			
		case cFormattingCancel:	/* FALL-THRU */
			fReaderRun();
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * GdgUpdate
 *
 * Update the gadget display.
 */
static void
GdgUpdate(void)
{
	IndexedColorType saveFore=WinSetForeColor(f);
	RectangleType r;

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cFormattingGDGF), &r);
	WinDrawRectangle(&r, 0);
	WinSetForeColor(UIColorGetTableEntryIndex(UIObjectFrame));
	WinDrawRectangleFrame(simpleFrame, &r);

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cFormattingGDGB), &r);
	WinSetForeColor(b);
	WinDrawRectangle(&r, 0);
	WinSetForeColor(UIColorGetTableEntryIndex(UIObjectFrame));
	WinDrawRectangleFrame(simpleFrame, &r);

	WinSetForeColor(saveFore);
}
