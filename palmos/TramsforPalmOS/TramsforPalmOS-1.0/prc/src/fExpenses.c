/*
 * fExpenses.c
 */
#include "Include.h"

/* macros */
#define LOffDate 16
#define LOffCategory (LOffDate+28)
#define LOffCurrency (LOffCategory+70)
#define ExpensesCategoryLength 64

/* protos */
static void ExpensesListDraw(Int16, RectangleType *, Char **);
static void ExpensesCategoryListDraw(Int16, RectangleType *, Char **);
static void InitExpense(void);
static void UpdateCurrencyTrigger(void);
static void UpdateDateTrigger(void);
static void UpdateExpCatPopup(UInt16);
static void UpdateCommentMandatory(void);
static void SetMandatoryComment(void);
static void CloseForm(void);
static void LoadFromExpense(void);

/* globals */
static UInt16 focusBand[]={ cExpensesAmount, cExpensesComment, 0 };
static Expense expense;
static DB db;
static Char trgCurrency[4], trgDate[dateStringLength], txtTitle[dmDBNameLength];
static Char trgExpCat[ExpensesCategoryLength];
static UInt16 recordIdx, expCatIdx, terIndex;
static DateFormatType dateFormat;
static Boolean commentMandatory;
static Char lockedFlag;

/*
 * ExpensesListDraw
 */
static void
ExpensesListDraw(Int16 itemNum, RectangleType *rct, Char **unused)
{
	MemHandle mh=DBGetRecord(&db, itemNum);
	Expense e;
	Char *p;
	Char buffer[20];
	UInt16 eIdx;
	Int16 fLen;
	Boolean fit;

	ExpensesMHToRecord(&e, mh);

	WinEraseRectangle(rct, 0);

	StrPrintF(buffer, "%u.", itemNum+1);
	eIdx=StrLen(buffer);
	fLen=FntCharsWidth(buffer, eIdx);
	WinDrawChars(buffer, eIdx, rct->topLeft.x+LOffDate-fLen-4, rct->topLeft.y);

	switch (dateFormat) {
	case dfDMYWithSlashes:
		StrPrintF(buffer, "%u/%u", e.date.day, e.date.month);
		break;
	case dfDMYWithDots:
		StrPrintF(buffer, "%u.%u", e.date.day, e.date.month);
		break;
	case dfDMYWithDashes:
		StrPrintF(buffer, "%u-%u", e.date.day, e.date.month);
		break;
	case dfYMDWithDots:
		StrPrintF(buffer, "%u.%u", e.date.month, e.date.day);
		break;
	case dfYMDWithDashes:
		StrPrintF(buffer, "%u-%u", e.date.month, e.date.day);
		break;
	default:	// dfYMDWithSlashes and dfMDYWithSlashes
		StrPrintF(buffer, "%u/%u", e.date.month, e.date.day);
		break;
	}

	eIdx=StrLen(buffer);
	fLen=FntCharsWidth(buffer, eIdx);
	WinDrawChars(buffer, eIdx, rct->topLeft.x+LOffCategory-fLen-4, rct->topLeft.y);

	if ((eIdx=ExpCatGetIndex(e.expCat))==dmMaxRecordIndex)
		StrCopy(buffer, "-- unknown --");
	else {
		mh=DBGetRecord(&dbExpCat, eIdx);
		p=(MemHandleLock(mh))+2;
		MemSet(buffer, sizeof(buffer), 0);
		StrNCopy(buffer, p, sizeof(buffer)-1);
		MemHandleUnlock(mh);
	}

	fLen=LOffCurrency-LOffCategory;
	eIdx=StrLen(buffer);
	FntCharsInWidth(buffer, &fLen, &eIdx, &fit);

	WinDrawChars(buffer, eIdx, rct->topLeft.x+LOffCategory, rct->topLeft.y);

	if ((p=LocIndexToCurrency(e.currency)))
		WinDrawChars(p, 3, rct->topLeft.x+LOffCurrency, rct->topLeft.y);

	MemSet(buffer, sizeof(buffer), 0);
	StrNCopy(buffer, e.amount, sizeof(buffer)-1);
	fLen=120;
	eIdx=StrLen(buffer);
	FntCharsInWidth(buffer, &fLen, &eIdx, &fit);
	WinDrawChars(buffer, eIdx, rct->topLeft.x+155-fLen, rct->topLeft.y);
}

/*
 * ExpensesCategoryListDraw
 */
static void
ExpensesCategoryListDraw(Int16 itemNum, RectangleType *rct, Char **unused)
{
	MemHandle mh=DBGetRecord(&dbExpCat, itemNum);
	Char *p;
	UInt16 len;

	p=MemHandleLock(mh);
	len=(UInt16)*p;
	WinEraseRectangle(rct, 0);

	WinDrawChars(p+2, len, rct->topLeft.x, rct->topLeft.y);
	MemHandleUnlock(mh);
}

/*
 * InitExpense
 *
 * Initialize an expense.
 */
static void
InitExpense(void)
{
	ExpensesInit(&expense);
	recordIdx=dmMaxRecordIndex;
}

/*
 * fExpensesRun
 *
 * Show the Expenses form.
 *
 *  -> idx		TER index.
 */
void
fExpensesRun(UInt16 idx)
{
	terIndex=idx;
	MemSet(txtTitle, sizeof(txtTitle), 0);
	StrNCopy(txtTitle, terList[idx].name+6, dmDBNameLength);
	lockedFlag=terList[idx].name[5];
	DBInit(&db, terList[idx].name);
	if (DBOpen(&db, dmModeReadWrite, true)==false) {
		FrmGotoForm(fMain);
		return;
	}

	expCatIdx=0xffff;
	InitExpense();
	dateFormat=PrefGetPreference(prefDateFormat);
	FrmGotoForm(fExpenses);
}

/*
 * fExpensesInit
 */
Boolean
fExpensesInit(void)
{
	ListType *lst=UIObjectGet(cExpensesList);

	LstSetDrawFunction(lst, ExpensesListDraw);
	LstSetListChoices(lst, NULL, DmNumRecordsInCategory(db.db, dmAllCategories));
	LstSetSelection(lst, noListSelection);

	lst=UIObjectGet(cExpensesCategoryList);
	LstSetDrawFunction(lst, ExpensesCategoryListDraw);
	LstSetListChoices(lst, NULL, DmNumRecordsInCategory(dbExpCat.db, dmAllCategories));

	UpdateExpCatPopup(0);
	LoadFromExpense();
	SetMandatoryComment();

	if (lockedFlag=='L')
		UIObjectShow(cExpensesLocked);
	else {
		UIObjectShow(cExpensesNew);
		UIObjectShow(cExpensesDelete);
		UIObjectShow(cExpensesSave);
	}

	return true;
}

/*
 * fExpensesEH
 */
Boolean
fExpensesEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		FrmSetTitle(currentForm, txtTitle);
		UpdateCommentMandatory();
		return true;

	case frmCloseEvent:
		CloseForm();
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cExpensesBack:
			if (lockedFlag=='L')
				FrmGotoForm(fMain);
			else {
				if (FrmAlert(aGoBackOK)==0)
					FrmGotoForm(fMain);
			}
			return true;

		case cExpensesNew:
			InitExpense();
			LoadFromExpense();
			expCatIdx=0xffff;
			UpdateExpCatPopup(0);
			UpdateCommentMandatory();
			FrmDrawForm(currentForm);
			return true;

		case cExpensesSave:
			{
				Char *p=UIFieldGetText(cExpensesAmount);
				ListType *lst=UIObjectGet(cExpensesList);
				MemHandle mh;
				DateType dSave;
				UInt32 uid;
				UInt16 rIdx, flg;
				Char ecCode[4];

				if ((p==NULL) || (StrLen(p)==0)) {
					FrmAlert(aEnterAmount);
					return true;
				}

				StrNCopy(expense.amount, p, ExpensesAmountLength);
				p=UIFieldGetText(cExpensesComment);
				if (p && StrLen(p)>0)
					StrNCopy(expense.comment, p, ExpensesCommentLength);
				else if (commentMandatory) {
					FrmAlert(aCommentMandatory);
					UIObjectFocus(cExpensesComment);
					return true;
				}

				if (CtlGetValue(UIObjectGet(cExpensesReceipt)))
					expense.flags=EFlgOriginalReceipt;
				else
					expense.flags=0;

				mh=ExpensesRecordToMH(&expense);
				uid=DBSetRecord(&db, recordIdx, dmUnfiledCategory, MemHandleLock(mh), MemHandleSize(mh), ExpensesSort);
				MemHandleFree(mh);

				LstSetListChoices(lst, NULL, DmNumRecordsInCategory(db.db, dmAllCategories));

				if (DmFindRecordByID(db.db, uid, &rIdx)==0) {
					LstMakeItemVisible(lst, rIdx);
					LstSetSelection(lst, rIdx);
				}
				
				state.countryIndex=expense.country;
				state.currencyIndex=expense.currency;
				dSave=expense.date;
				flg=expense.flags&EFlgOriginalReceipt;
				MemMove(ecCode, expense.expCat, 4);
				InitExpense();
				MemMove(expense.expCat, ecCode, 4);
				expense.date=dSave;
				expense.flags=flg;
				LoadFromExpense();
				FrmDrawForm(currentForm);
				UIObjectFocus(cExpensesCategoryPopup);
			}
			return true;

		case cExpensesDelete:
			{
				ListType *lst=UIObjectGet(cExpensesList);
				UInt16 rIdx;

				if ((rIdx=LstGetSelection(lst))!=noListSelection) {
					if (FrmAlert(aDeleteExpense)==0) {
						DmRemoveRecord(db.db, rIdx);

						LstSetListChoices(lst, NULL, DmNumRecordsInCategory(db.db, dmAllCategories));
						if (rIdx>0) {
							rIdx--;

							LstMakeItemVisible(lst, rIdx);
							LstSetSelection(lst, rIdx);
						}

						InitExpense();
						LoadFromExpense();
						FrmDrawForm(currentForm);
					}
				}
			}
			break;


		case cExpensesCurrency:
			if (fCountryRun(&expense.country, &expense.currency)==true)
				UpdateCurrencyTrigger();

			return true;

		case cExpensesDate:
			{
				Int16 y=expense.date.year+firstYear, m=expense.date.month, d=expense.date.day;
				if (SelectDay(selectDayByDay, &m, &d, &y, "Select Expense Date")==true) {
					expense.date.year=y-firstYear;
					expense.date.month=m;
					expense.date.day=d;
					UpdateDateTrigger();
				}
			}
			return true;
		}
		break;

	case lstSelectEvent:
		if (ev->data.lstSelect.selection!=noListSelection) {
			MemHandle mh=DBGetRecord(&db, ev->data.lstSelect.selection);
			recordIdx=ev->data.lstSelect.selection;
			ExpensesMHToRecord(&expense, mh);
			LoadFromExpense();
			return true;
		}
		break;

	case popSelectEvent:
		UpdateExpCatPopup(ev->data.popSelect.selection);
		UpdateCommentMandatory();
		SetMandatoryComment();
		return true;

	case keyDownEvent:
		if (ev->data.keyDown.chr==pageUpChr) {
			LstScrollList(UIObjectGet(cExpensesList), winUp, 3);
			return false;
		} else if (ev->data.keyDown.chr==pageDownChr) {
			LstScrollList(UIObjectGet(cExpensesList), winDown, 3);
			return false;
		}

		return UIFieldRefocus(focusBand, ev->data.keyDown.chr);

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdateCurrencyTrigger
 */
static void
UpdateCurrencyTrigger(void)
{
	Char *p;

	if ((p=LocIndexToCurrency(expense.currency))==NULL)
		MemMove(trgCurrency, "???", 3);
	else {
		MemMove(trgCurrency, p, 3);
	}
	trgCurrency[3]=0;

	CtlSetLabel(UIObjectGet(cExpensesCurrency), trgCurrency);
}

/*
 * UpdateDateTrigger
 */
static void
UpdateDateTrigger(void)
{
	DateToAscii((UInt8)expense.date.month, (UInt8)expense.date.day, (UInt16)expense.date.year+firstYear, dateFormat, trgDate);
	CtlSetLabel(UIObjectGet(cExpensesDate), trgDate);
}

/*
 * UpdateExpCatPopup
 *
 * Update the popup text with the chosen index.
 *
 *  -> nIdx		New index.
 */
static void
UpdateExpCatPopup(UInt16 nIdx)
{
	MemHandle mh;
	Char *p;
	UInt16 len;

	if ((nIdx==noListSelection) || (nIdx==expCatIdx)) {
		commentMandatory=false;
		return;
	}

	expCatIdx=nIdx;
	mh=DBGetRecord(&dbExpCat, expCatIdx);
	p=MemHandleLock(mh);
	len=(UInt16)*p++;
	if (*p++)
		commentMandatory=true;
	else
		commentMandatory=false;

	StrNCopy(trgExpCat, p, sizeof(trgExpCat));
	p+=len+1;
	MemMove(expense.expCat, p, 4);
	MemHandleUnlock(mh);

	CtlSetLabel(UIObjectGet(cExpensesCategoryPopup), trgExpCat);
	LstSetSelection(UIObjectGet(cExpensesCategoryList), expCatIdx);
}

/*
 * UpdateCommentMandatory
 *
 * Update the "Comment is mandatory" visual.
 */
static void
UpdateCommentMandatory(void)
{
	FontID fID=FntSetFont(symbolFont);

	if (commentMandatory) {
		if (ui->colorEnabled) {
			RGBColorType rgb={ 0, 255, 0, 0 };
			IndexedColorType ict;

			ict=WinSetTextColor(WinRGBToIndex(&rgb));

			WinDrawChars("\x13", 1, 0, 60);

			WinSetTextColor(ict);
		} else
			WinDrawChars("\x13", 1, 0, 60);
	} else {
		RectangleType rct;

		RctSetRectangle(&rct, 0, 60, FntCharWidth('\x13'), FntCharHeight());
		WinEraseRectangle(&rct, 0);
	}

	FntSetFont(fID);
}

/*
 * SetMandatoryComment
 *
 * Set a mandatory comment.
 */
static void
SetMandatoryComment(void)
{
	Char *p;

	if (prefs.flags&PFlgAutoComment && *prefs.autoComment) {
		if ((p=UIFieldGetText(cExpensesComment))) {
			if (StrLen(p)==0)
				UIFieldSetText(cExpensesComment, prefs.autoComment);
		} else
			UIFieldSetText(cExpensesComment, prefs.autoComment);
	}
}

/*
 * CloseForm
 *
 * Stuff to do before form closes.
 */
static void
CloseForm(void)
{
	state.countryIndex=expense.country;
	state.currencyIndex=expense.currency;

	if (db.db) {
		DBClose(&db);

		if (appStopped==true)
			StrCopy(state.currentTER, db.name);
		else
			MemSet(state.currentTER, dmDBNameLength, 0);
	} else
		*state.currentTER=0;
}

/*
 * LoadFromExpense
 *
 * Load form fields from expense.
 */
static void
LoadFromExpense(void)
{
	UInt16 eIdx;

	UpdateCurrencyTrigger();
	UpdateDateTrigger();

	CtlSetValue(UIObjectGet(cExpensesReceipt), expense.flags&EFlgOriginalReceipt);
	if (*expense.amount)
		UIFieldSetText(cExpensesAmount, expense.amount);
	else
		UIFieldClear(cExpensesAmount);

	if (*expense.comment)
		UIFieldSetText(cExpensesComment, expense.comment);
	else
		UIFieldClear(cExpensesComment);

	UIObjectFocus(cExpensesCategoryPopup);

	eIdx=ExpCatGetIndex(expense.expCat);
	expCatIdx=0xffff;
	UpdateExpCatPopup((eIdx==dmMaxRecordIndex) ? 0 : eIdx);

}
