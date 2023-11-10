/*
 * Expenses.c
 */
#include "Include.h"

/*
 * ExpensesSort
 *
 * Sort based on date.
 */
Int16
ExpensesSort(void *r1, void *r2, Int16 u1, SortRecordInfoType *u2, SortRecordInfoType *u3, MemHandle u4)
{
	UInt16 *e1=(UInt16 *)r1, *e2=(UInt16 *)r2;

	if (*e1<*e2)
		return -1;

	if (*e1>*e2)
		return 1;

	return 0;
}

/*
 * ExpensesInit
 *
 * Initialize an expense.
 *
 *  -> e		Expense.
 */
void
ExpensesInit(Expense *e)
{
	DateTimeType dtt;

	MemSet(e, sizeof(Expense), 0);

	e->country=state.countryIndex;
	e->currency=state.currencyIndex;
	TimSecondsToDateTime(TimGetSeconds(), &dtt);
	e->date.year=(dtt.year-firstYear);
	e->date.month=dtt.month;
	e->date.day=dtt.day;

	e->flags=EFlgOriginalReceipt;
}

/*
 * ExpensesRecordToMH
 *
 * Convert an Expense record into a MH.
 *
 *  -> e		Expense.
 *
 * Returns the packed record as a MH.
 */
MemHandle
ExpensesRecordToMH(Expense *e)
{
	/* rLen is: sizeof(date)+sizeof(country)+sizeof(currency)+4+
	 *	    sizeof(flags)+sizeof(ExpCat)+countrycode (2) +
	 *	    2 term bytes */
	UInt32 rLen=sizeof(DateType)+2+2+2+4+4+2+2;
	Char *expCat;
	MemHandle mh;
	UInt8 *d;
	const UInt8 *c;
	Char *p;

	if (*e->amount)
		rLen+=StrLen(e->amount);

	if (*e->comment)
		rLen+=StrLen(e->comment);

	if ((expCat=ExpCatGetCategory(e->expCat))!=NULL)
		rLen+=StrLen(expCat)+1;
	else
		rLen++;

	mh=MemHandleNew(rLen);
	ErrFatalDisplayIf(mh==NULL, "(ExpensesRecordToMH) Out of memory");
	d=MemHandleLock(mh);

	*(DateType *)d=e->date;
	d+=sizeof(DateType);
	*(UInt16 *)d=e->country;
	d+=2;
	c=LocIndexToCountryCode(e->country);
	*d++=c[0];
	*d++=c[1];
	*(UInt16 *)d=e->currency;
	d+=2;
	if ((p=LocIndexToCurrency(e->currency))==NULL) {
		*d++='?';
		*d++='?';
		*d++='?';
	} else {
		*d++=*p++;
		*d++=*p++;
		*d++=*p++;
	}
	*d++=0;
	*(UInt16 *)d=e->flags;
	d+=2;
	MemMove(d, e->expCat, 4);
	d+=4;

	if (*e->amount) {
		StrCopy(d, e->amount);
		d+=StrLen(d);
	}
	*d++=0;

	if (*e->comment) {
		StrCopy(d, e->comment);
		d+=StrLen(d);
	}

	*d++=0;
	if (expCat) {
		StrCopy(d, expCat);
		d+=StrLen(expCat);
		MemPtrFree(expCat);
	}
	*d=0;

	MemHandleUnlock(mh);
	return mh;
}

/*
 * ExpensesMHToRecord
 *
 * Convert a packed record (a MH) to an Expense.
 *
 * <-  e		Expense.
 *  -> mh		MH.
 */
void
ExpensesMHToRecord(Expense *e, MemHandle mh)
{
	UInt8 *d=MemHandleLock(mh);

	MemSet(e, sizeof(Expense), 0);

	e->date=(*(DateType *)d);
	d+=sizeof(DateType);
	e->country=*(UInt16 *)d;
	d+=4;	// +2 (UInt16), +2 country code
	e->currency=*(UInt16 *)d;
	d+=6;	// +2 (UInt16), +4 currency string
	e->flags=*(UInt16 *)d;
	d+=2;
	MemMove(e->expCat, d, 4);
	d+=4;

	if (*d) {
		StrNCopy(e->amount, d, ExpensesAmountLength);
		d+=StrLen(d);
	}
	d++;

	if (*d) {
		StrNCopy(e->comment, d, ExpensesCommentLength);
		d+=StrLen(d);
	}

	MemHandleUnlock(mh);
}
