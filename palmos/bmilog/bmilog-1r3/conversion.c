/*
**	conversion.c
**
**	Code to drive the Conversion form.
*/
#include "bmilog.h"

/* protos */
static void Solve(void);

/* globals */
static UInt16 focusBand[]={ fConversionKg, fConversionFeet, fConversionInch, fConversionCm, fConversionLbs, fConversionKg, 0 };

/*
**	ConversionInit
**
**	Initialize the Conversion form.
**
**	Returns true if successful, false otherwise.
*/
Boolean
ConversionInit(void)
{
	UIFieldFocus(fConversionFeet);
	return true;
}

/*
**	ConversionEventHandler
**
**	Event handler for Conversion form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
Boolean
ConversionEventHandler(EventPtr ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fConversionDone:
			FrmGotoForm(fMain);
			return true;
		case fConversionSolve:
			Solve();
			return true;
		case fConversionClear:
			UIFieldClear(fConversionKgA);
			UIFieldClear(fConversionLbsA);
			UIFieldClear(fConversionCmAInch);
			UIFieldClear(fConversionCmAFeet);
			UIFieldClear(fConversionFeetInchA);
			UIFieldClear(fConversionKg);
			UIFieldClear(fConversionLbs);
			UIFieldClear(fConversionCm);
			UIFieldClear(fConversionInch);
			UIFieldClear(fConversionFeet);
			return true;
		}
		break;

	case keyDownEvent:
		return UIRefocus(focusBand, ev->data.keyDown.chr);

	default:
		break;
	}

	return false;
}

/*
**	Solve
**
**	Solve the various conversions.
*/
static void
Solve(void)
{
	UInt32 n1=0, n2=0;
	Char *num1, *num2;
	Char buffer[32];

	/* Feet/Inch to cm */
	if ((num1=UIFieldGetText(fConversionFeet)))
		n1=StrAToI(num1);

	if ((num2=UIFieldGetText(fConversionInch)))
		n2=StrAToI(num2);

	if (n1 || n2) {
		if (n1==0)
			UIFieldSetText(fConversionFeet, "0");

		if (n2==0)
			UIFieldSetText(fConversionInch, "0");

		n1*=12;
		n1+=n2;
		StrPrintF(buffer, "%lu", (n1*254)/100);
		UIFieldSetText(fConversionFeetInchA, buffer);
	}

	/* Cm to Feet/Inch */
	if ((num1=UIFieldGetText(fConversionCm))) {
		UtilsCmToFeetInch(StrAToI(num1), &n1, &n2);

		StrPrintF(buffer, "%lu", n1);
		UIFieldSetText(fConversionCmAFeet, buffer);

		StrPrintF(buffer, "%lu", n2);
		UIFieldSetText(fConversionCmAInch, buffer);
	}

	/* Lbs to Kg */
	if ((num1=UIFieldGetText(fConversionLbs))) {
		n1=StrAToI(num1)*45359;
		StrPrintF(buffer, "%lu", n1/100000);
		UIFieldSetText(fConversionLbsA, buffer);
	}

	/* Kg to Lbs */
	if ((num1=UIFieldGetText(fConversionKg))) {
		n1=StrAToI(num1)*100000;
		StrPrintF(buffer, "%lu", n1/45359);
		UIFieldSetText(fConversionKgA, buffer);
	}
}
