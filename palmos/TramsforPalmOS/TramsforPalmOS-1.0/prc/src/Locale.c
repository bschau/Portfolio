/*
 * Locale.c
 */
#include "Include.h"

/* macros */
typedef struct {
	MemHandle resH;
	Int16 count;
	Char *data;
	MemHandle stringsH;
	Char **strings;
} Loc;

/* protos */
static void Open(Loc *, UInt16);
static void Close(Loc *);

/* globals */
static Loc country, currency;
static MemHandle xRefH, cCodesH;
static Int16 *xRef;
static UInt8 *cCodes;

/*
 * Open
 *
 * Open the resource, form strings etc.
 *
 * <-  loc		Pointer to destination Loc.
 *  -> id		Resource ID.
 */
static void
Open(Loc *loc, UInt16 id)
{
	Char *p;

	MemSet(loc, sizeof(Loc), 0);

	loc->resH=DmGetResource('LOCX', id);
	ErrFatalDisplayIf(loc->resH==NULL, "(Open) Cannot find LOCX resource");
	p=MemHandleLock(loc->resH);

	/* now some dirty work .. */
	loc->count=StrAToI(p);
	p+=StrLen(p)+1;

	loc->data=p;

	loc->stringsH=SysFormPointerArrayToStrings(loc->data, loc->count);
	ErrFatalDisplayIf(loc->stringsH==NULL, "(Open) Cannot form LOCX String List");
	loc->strings=MemHandleLock(loc->stringsH);
}

/*
 * LocOpen
 *
 * Open locale.
 */
void
LocOpen(void)
{
	Open(&country, LOCXCountryID);
	Open(&currency, LOCXCurrencyID);

	xRefH=DmGetResource('LOCX', LOCXXrefID);
	ErrFatalDisplayIf(xRefH==NULL, "(LocOpen) Cannot find LOCX Xref resource");
	xRef=MemHandleLock(xRefH);

	cCodesH=DmGetResource('LOCX', LOCXCountryCodesID);
	ErrFatalDisplayIf(cCodesH==NULL, "(LocOpen) Cannot find LOCX Country Codes resource");
	cCodes=MemHandleLock(cCodesH);
}

/*
 * Close
 *
 * Close the loc.
 *
 *  -> loc		Loc.
 */
static void
Close(Loc *loc)
{
	if (loc->stringsH)
		MemHandleFree(loc->stringsH);

	if (loc->resH) {
		MemHandleUnlock(loc->resH);
		DmReleaseResource(loc->resH);
	}
}

/*
 * LocClose
 *
 * Close locale.
 */
void
LocClose(void)
{
	if (cCodesH) {
		MemHandleUnlock(cCodesH);
		DmReleaseResource(cCodesH);
	}

	if (xRefH) {
		MemHandleUnlock(xRefH);
		DmReleaseResource(xRefH);
	}

	Close(&country);
	Close(&currency);
}

/*
 * LocValidate
 *
 * Validate/update State info if pointers run astray.
 *
 *  -> s		State.
 */
void
LocValidate(State *s)
{
	if (s->countryIndex>country.count)
		s->countryIndex=0;

	if (s->currencyIndex>currency.count)
		s->currencyIndex=0;
}

/*
 * LocCountryToIndex
 *
 * Given a country name, return index.
 *
 *  -> name		Country name.
 *
 * Returns index or -1 if not found.
 */
Int16
LocCountryToIndex(Char *name)
{
	Char *p=country.data;
	UInt16 idx;

	for (idx=0; idx<country.count; idx++) {
		if (StrCaselessCompare(p, name)==0)
			return idx;

		for (; *p; p++);
		p++;
	}

	return -1;
}

/*
 * LocIndexToCountry
 *
 * Given an index return country (or NULL).
 *
 *  -> index		Index.
 */
Char *
LocIndexToCountry(Int16 index)
{
	Char *p=country.data;

	if (index>country.count)
		return NULL;

	for (; index>0; index--) {
		for (; *p; p++);
		p++;
	}

	return p;
}

/*
 * LocIndexToCountryCode
 *
 * Given a country index return the country code.
 *
 *  -> index		Index.
 * 
 * Returns read only pointer to country code. 
 */
const UInt8 *
LocIndexToCountryCode(Int16 index)
{
	UInt32 l=index*2;

	return (cCodes+l);
}

/*
 * LocCurrencyToIndex
 *
 * Given a currency name, return index.
 *
 *  -> name		Currency name.
 *
 * Returns index or -1 if not found.
 */
Int16
LocCurrencyToIndex(Char *name)
{
	Char *p=currency.data;
	UInt16 idx;

	for (idx=0; idx<currency.count; idx++) {
		if (StrNCaselessCompare(p, name, 3)==0)
			return idx;

		for (; *p; p++);
		p++;
	}

	return -1;
}

/*
 * LocIndexToCurrency
 *
 * Given an index return currency (or NULL).
 *
 *  -> index		Index.
 */
Char *
LocIndexToCurrency(Int16 index)
{
	Char *p=currency.data;

	if (index>currency.count)
		return NULL;

	for (; index>0; index--) {
		for (; *p; p++);
		p++;
	}

	return p;
}

/*
 * LocCountryToCurrency
 *
 * Given a country index return currency index.
 *
 *  -> idx		Country index.
 *
 * Returns currency index or -1 if country index > no. of countries!
 */
Int16
LocCountryToCurrency(Int16 idx)
{
	if ((idx<0) || (idx>country.count))
		return -1;

	return xRef[idx];
}

/*
 * LocSetupList
 *
 * Setup the list as a country list.
 *
 *  -> id		List ID.
 *  -> idx		Selected index (or noListSelection).
 *  -> type		Currency list if true, country list otherwise.
 */
void
LocSetupList(UInt16 id, Int16 idx, Boolean type)
{
	ListType *lst=UIObjectGet(id);

	if (type)
		LstSetListChoices(lst, currency.strings, currency.count);
	else
		LstSetListChoices(lst, country.strings, country.count);

	LstSetSelection(lst, idx);
	LstMakeItemVisible(lst, idx);
}
