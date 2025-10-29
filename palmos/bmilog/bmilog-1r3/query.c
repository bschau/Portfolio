/*
**	query.c
**
**	Code to drive the Conversion forms.
*/
#include "bmilog.h"

/* protos */
static void SolveKgLbs(void);
static void SolveCmFeetInch(void);

/* globals */
static UInt16 focusBandFH[]={ fQueryFindHeightLbs, fQueryFindHeightKg, fQueryFindHeightLbs, 0 };
static UInt16 focusBandFW[]={ fQueryFindWeightInch, fQueryFindWeightCm, fQueryFindWeightFeet, fQueryFindWeightInch, 0 };

/*
**	QueryFindHeightInit
**
**	Initialize the Find Height form.
**
**	Returns true if successful, false otherwise.
*/
Boolean
QueryFindHeightInit(void)
{
	UIListSetPopup(fQueryFindHeightTableList, fQueryFindHeightTablePopup, prefs.bmiTable);

	if (prefs.format==FormatKgCm)
		UIFieldFocus(fQueryFindHeightKg);
	else
		UIFieldFocus(fQueryFindHeightLbs);

	return true;
}

/*
**	QueryFindHeightEventHandler
*/
Boolean
QueryFindHeightEventHandler(EventPtr ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fQueryFindHeightDone:
			FrmGotoForm(fMain);
			return true;
		case fQueryFindHeightSolve:
			SolveKgLbs();
			return true;
		case fQueryFindHeightClear:
			UIFieldClear(fQueryFindHeightOptimal);
			UIFieldClear(fQueryFindHeightKg);
			UIFieldClear(fQueryFindHeightLbs);
			if (prefs.format==FormatKgCm)
				UIFieldFocus(fQueryFindHeightKg);
			else
				UIFieldFocus(fQueryFindHeightLbs);

			return true;
		}
		break;

	case keyDownEvent:
		return UIRefocus(focusBandFH, ev->data.keyDown.chr);

	default:
		break;
	}

	return false;
}

/*
**	SolveKgLbs
**
**	Find optimum height given kg/lbs.
*/
static void
SolveKgLbs(void)
{
	UInt32 n1=0, n2=0, minFeet, maxFeet, minInch, maxInch, minCm, maxCm;
	double max=24.9, min;
	Char *num1, *num2, *cm, *feet, *inch;
	Char buffer[MaxQueryLength];
	FlpCompDouble minFcd, maxFcd;

	if ((num1=UIFieldGetText(fQueryFindHeightKg)))
		n1=StrAToI(num1);

	if ((num2=UIFieldGetText(fQueryFindHeightLbs)))
		n2=StrAToI(num2);

	if (n1==0 && n2==0)
		return;

	if (n1 && n2) {
		FrmAlert(aEitherKgOrLbs);
		return;
	}

	cm=MemHandleLock(resStrCm);
	feet=MemHandleLock(resStrFeet);
	inch=MemHandleLock(resStrInch);
	if (LstGetSelection(UIFormGetObject(fQueryFindHeightTableList))==BmiTableAmerican)
		min=(double)20;
	else
		min=(double)18.5;

	if (n1) {
		minFcd.d=(__ieee754_sqrt((double)n1/(double)max)*(double)100);
		maxFcd.d=(__ieee754_sqrt((double)n1/(double)min)*(double)100);
		minCm=(UInt32)minFcd.d;
		maxCm=(UInt32)maxFcd.d;
		UtilsCmToFeetInch(minCm, &minFeet, &minInch);
		UtilsCmToFeetInch(maxCm, &maxFeet, &maxInch);
		StrPrintF(buffer, "%lu - %lu %s\n%lu%s%lu%s - %lu%s%lu%s", minCm, maxCm, cm, minFeet, feet, minInch, inch, maxFeet, feet, maxInch, inch);
	} else {
		minFcd.d=__ieee754_sqrt((double)n2*(double)704.5/(double)max);
		maxFcd.d=__ieee754_sqrt((double)n2*(double)704.5/(double)min);
		minFeet=((UInt32)minFcd.d)/12;
		minInch=((UInt32)minFcd.d)%12;
		maxFeet=((UInt32)maxFcd.d)/12;
		maxInch=((UInt32)maxFcd.d)%12;
		minCm=(UInt32)(minFcd.d*(double)2.54);
		maxCm=(UInt32)(maxFcd.d*(double)2.54);
		StrPrintF(buffer, "%lu%s%lu%s - %lu%s%lu%s\n%lu - %lu %s", minFeet, feet, minInch, inch, maxFeet, feet, maxInch, inch, minCm, maxCm, cm);
	}
	UIFieldSetText(fQueryFindHeightOptimal, buffer);

	MemHandleUnlock(resStrInch);
	MemHandleUnlock(resStrFeet);
	MemHandleUnlock(resStrCm);
}

/*
**	QueryFindWeightInit
**
**	Initialize the Find Weight form.
**
**	Returns true if successful, false otherwise.
*/
Boolean
QueryFindWeightInit(void)
{
	UIListSetPopup(fQueryFindWeightTableList, fQueryFindWeightTablePopup, prefs.bmiTable);

	if (prefs.format==FormatKgCm)
		UIFieldFocus(fQueryFindWeightCm);
	else
		UIFieldFocus(fQueryFindWeightFeet);

	return true;
}

/*
**	QueryFindWeightEventHandler
*/
Boolean
QueryFindWeightEventHandler(EventPtr ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fQueryFindWeightDone:
			FrmGotoForm(fMain);
			return true;
		case fQueryFindWeightSolve:
			SolveCmFeetInch();
			return true;
		case fQueryFindWeightClear:
			UIFieldClear(fQueryFindWeightOptimal);
			UIFieldClear(fQueryFindWeightCm);
			UIFieldClear(fQueryFindWeightFeet);
			UIFieldClear(fQueryFindWeightInch);
			if (prefs.format==FormatKgCm)
				UIFieldFocus(fQueryFindWeightCm);
			else
				UIFieldFocus(fQueryFindWeightFeet);

			return true;
		}
		break;

	case keyDownEvent:
		return UIRefocus(focusBandFW, ev->data.keyDown.chr);

	default:
		break;
	}

	return false;
}

/*
**	SolveCmFeetInch
**
**	Find optimum weight given cm/feet+inch.
*/
static void
SolveCmFeetInch(void)
{
	UInt32 n1=0, n2=0, minKg, maxKg, minLbs, maxLbs;
	double max=24.9, min;
	Char *num1, *num2, *kg, *lbs;
	Char buffer[MaxQueryLength];
	FlpCompDouble minFcd, maxFcd;

	if ((num1=UIFieldGetText(fQueryFindWeightCm)))
		n1=StrAToI(num1);

	if ((num2=UIFieldGetText(fQueryFindWeightFeet))) {
		n2=StrAToI(num2)*12;
		if ((num2=UIFieldGetText(fQueryFindWeightInch)))
			n2+=StrAToI(num2);
		else 
			UIFieldSetText(fQueryFindWeightInch, "0");
	}

	if (n1==0 && n2==0)
		return;

	if (n1 && n2) {
		FrmAlert(aEitherCmOrFeetInch);
		return;
	}

	kg=MemHandleLock(resStrKg);
	lbs=MemHandleLock(resStrLbs);
	if (LstGetSelection(UIFormGetObject(fQueryFindWeightTableList))==BmiTableAmerican)
		min=(double)20;
	else
		min=(double)18.5;

	if (n1) {
		minFcd.d=(((double)n1*(double)n1)/(double)10000)*min;
		maxFcd.d=(((double)n1*(double)n1)/(double)10000)*max;
		minKg=(UInt32)minFcd.d;
		maxKg=(UInt32)maxFcd.d;
		minLbs=(UInt32)(minFcd.d/(double)0.45359);
		maxLbs=(UInt32)(maxFcd.d/(double)0.45359);
		StrPrintF(buffer, "%lu - %lu %s\n%lu - %lu %s", minKg, maxKg, kg, minLbs, maxLbs, lbs);
	} else {
		minFcd.d=((((double)n2*(double)n2)*min)/(double)704.5);
		maxFcd.d=((((double)n2*(double)n2)*max)/(double)704.5);
		minLbs=(UInt32)minFcd.d;
		maxLbs=(UInt32)maxFcd.d;
		minKg=(UInt32)(minFcd.d*(double)0.45359);
		maxKg=(UInt32)(maxFcd.d*(double)0.45359);
		StrPrintF(buffer, "%lu - %lu %s\n%lu - %lu %s", minLbs, maxLbs, lbs, minKg, maxKg, kg);
	}
	UIFieldSetText(fQueryFindWeightOptimal, buffer);

	MemHandleUnlock(resStrLbs);
	MemHandleUnlock(resStrKg);
}
