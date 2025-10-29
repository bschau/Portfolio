/*
 * fMain.c
 */
#include "Include.h"

/* macros */
#define CornerDiam 5

/* protos */
static void PickupNames(Char *, Char **, Char **);
static void DrawHelpButton(void);
static Boolean GadgetHandler(UInt16, UInt16);
static void fBluetoothRun(void);
static Boolean fBluetoothEH(EventType *);

/* globals */
static UInt16 focusBand[]={ cMainName, cMainPhone, 0 };

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	Char *n, *p;
	UInt16 nLen, pLen;
	
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);

		if (StrLen(state.name)>0) {
			FieldType *fld=UIObjectGet(cMainName);

			UIFieldSetText(cMainName, state.name);
			FldSetSelection(fld, 0, FldGetTextLength(fld));
			FrmGlueNavObjectTakeFocus(currentForm, cMainName);
		} else if (StrLen(state.phone)>0) {
			FieldType *fld=UIObjectGet(cMainPhone);

			UIFieldSetText(cMainPhone, state.phone);
			FldSetSelection(fld, 0, FldGetTextLength(fld));
			FrmGlueNavObjectTakeFocus(currentForm, cMainPhone);
		}
	case frmUpdateEvent:
		DrawHelpButton();
		return true;

	case frmObjectFocusTakeEvent:
		if (ui->oneHandedEnabled) {
			FieldType *fld;

			switch (ev->data.frmObjectFocusTake.objectID) {
			case cMainName:
				fld=UIObjectGet(cMainName);

				FldSetSelection(fld, 0, FldGetTextLength(fld));
				break;

			case cMainPhone:
				fld=UIObjectGet(cMainPhone);

				FldSetSelection(fld, 0, FldGetTextLength(fld));
				break;
			}
		}
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainSearch:
			if ((n=UIFieldGetText(cMainName)))
				nLen=StrLen(n);
			else
				nLen=0;

			if ((p=UIFieldGetText(cMainPhone)))
				pLen=StrLen(p);
			else
				pLen=0;

			if ((nLen==0) && (pLen==0))
				return true;

			if ((nLen!=0) && (pLen!=0)) {
				switch (FrmCustomAlert(aWhichLookup, n, p, NULL)) {
				case 0:
					UIFieldClear(cMainPhone);
					pLen=0;
					break;
				case 1:
					UIFieldClear(cMainName);
					nLen=0;
					break;
				default:
					return true;
				}
			}

			MemSet(state.name, sizeof(state.name), 0);
			MemSet(state.phone, sizeof(state.phone), 0);

			if (nLen>0) {
				/* Two term zeros are needed in PickupNames */
				Char buffer[SearchNameLength+2];
				Char *givenName, *surName;

				MemSet(buffer, sizeof(buffer), 0);
				StrCopy(state.name, n);

				StrToLower(buffer, n);
				PickupNames(buffer, &givenName, &surName);
				if ((givenName!=NULL) || (surName!=NULL))
					NSSetup(givenName, surName);
			} else {
				if (pLen>3) {
					MemHandle mh;
					UInt16 offset;

					StrCopy(state.phone, p);
					if (PhoneSearch(p, &mh, &offset)==true) {
						fDetailsRun(mh, offset);
						MemHandleFree(mh);
					} else
						FrmAlert(aNoMatch);
				} else
					FrmAlert(aPhoneTooShort);
			}
			return true;
		}
		break;

	case penDownEvent:
		return GadgetHandler(ev->screenX, ev->screenY);

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainBluetooth:
			if (PMBtToggleEnabled(NULL, NULL)==true)
				fBluetoothRun();
			else
				FrmAlert(aNoBtToggle);

			return true;

		case mMainAbout:
			{
				FormSave frm;

				UIFormPrologue(&frm, fAbout, NULL);
				UIFormEpilogue(&frm, NULL, 0);
			}
			return true;
		}
		break;

	case keyDownEvent:
		return UIFieldRefocus(focusBand, ev->data.keyDown.chr);

	default:
		break;
	}

	return false;
}

/*
 * PickupNames
 *
 * Parse a string into firstName, LastName.
 *
 *  -> src	Source string.
 *  -> fN	First Name.
 *  -> lN	Last Name.
 */
static void
PickupNames(Char *src, Char **fN, Char **lN)
{
	*fN=NULL;
	*lN=NULL;

	for (; *src; src++) {
		if (*src>' ')
			break;
	}

	if (!*src)
		return;

	*lN=src;
	for (; *src; src++) {
		if (*src<=' ')
			break;
	}

	*src++='\x00';
	if (!*src)
		return;

	for (; *src; src++) {
		if (*src>' ')
			break;
	}
	if (!*src)
		return;

	*fN=src;
	for (; *src; src++) {
		if (*src<=' ')
			break;
	}

	*src='\x00';
}

/*
 * DrawHelpButton
 *
 * Draw the help button.
 */
static void
DrawHelpButton(void)
{
	IndexedColorType t;
	RectangleType r;
	FontID fID;

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cMainHelp), &r);

	WinEraseRectangle(&r, CornerDiam);

	t=WinSetTextColor(ui->foreColor);
	fID=FntSetFont(symbolFont);
	WinDrawChar('\x0a', r.topLeft.x+3, r.topLeft.y);
	FntSetFont(fID);
	WinSetTextColor(t);
}

/*
 * GadgetHandler
 *
 * Handle a gadget tap.
 *
 *  -> x		Point X
 *  -> y		Point Y
 *
 * Returns true if gadget was handled, false otherwise.
 */
static Boolean
GadgetHandler(UInt16 x, UInt16 y)
{
	RectangleType r;
	Boolean wasInBounds, nowInBounds, penDown;
	IndexedColorType b, f;
	WinDrawOperation w;

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cMainHelp), &r);
	if (RctPtInRectangle(x, y, &r)) {
		wasInBounds=true;
		
		b=WinSetBackColor(ui->backColor);
		f=WinSetForeColor(ui->foreColor);
		
		w=WinSetDrawMode(winSwap);
		wasInBounds=false;
		do {
			PenGetPoint(&x, &y, &penDown);
			nowInBounds=RctPtInRectangle(x, y, &r);
			if (nowInBounds!=wasInBounds) {
				WinPaintRectangle(&r, CornerDiam);
				wasInBounds=nowInBounds;
			}
		} while (penDown);
		DrawHelpButton();

		WinSetDrawMode(w);
		WinSetForeColor(f);
		WinSetBackColor(b);
		FrmHelp(hMain);
		return true;
	}

	return false;
}

/*
 * fBluetoothRun
 *
 * Show and handle the preferences dialog.
 */
static void
fBluetoothRun(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fBluetooth, fBluetoothEH);

	if (prefs.flags&PFlgBluetooth)
		CtlSetValue(UIObjectGet(cBluetoothBluetooth), 1);

	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * fBluetoothEH
 */
static Boolean
fBluetoothEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent && ev->data.ctlSelect.controlID==cBluetoothOK) {
		if ((CtlGetValue(UIObjectGet(cBluetoothBluetooth))))
			prefs.flags=PFlgBluetooth;
		else
			prefs.flags=0;
	}

	return false;
}
