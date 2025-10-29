#include "Include.h"

/* protos */
static Int16 QSortCompare(void *, void *, Int32);

/* globals */
static MemHandle terListH;

/*
 * QSortCompare
 */
static Int16
QSortCompare(void *r1, void *r2, Int32 u)
{
	return StrCaselessCompare((Char *)r1, (Char *)r2);
}

/*
 * TLBuildList
 *
 * Build the TER list.
 */
void
TLBuildList(void)
{
	TLDestroyList();
	SysCreateDataBaseList('DATA', (UInt32)CRID, &terCnt, &terListH, false);
	if (terListH) {
		terList=MemHandleLock(terListH);
		TLSort();
	}
}

/*
 * TLSort
 * 
 * Sort the TER list.
 */
void
TLSort(void)
{
	SysQSort(terList, terCnt, sizeof(SysDBListItemType), QSortCompare, 0);
}

/*
 * TLDestroyList
 *
 * Destroy the TER list.
 */
void
TLDestroyList(void)
{
	if (terListH) {
		MemHandleFree(terListH);
		terListH=NULL;
		terList=NULL;
		terCnt=0;
	}
}

/*
 * TLHasTer
 *
 * Check if the TER is already on the list.
 *
 *  -> name		TER name.
 *
 * Returns true if the TER is found, false otherwise.
 */
Boolean
TLHasTer(Char *name)
{
	UInt16 idx;

	name+=6;
	for (idx=0; idx<terCnt; idx++) {
		if (StrCaselessCompare(name, terList[idx].name+6)==0)
			return true;
	}

	return false;
}

/*
 * TLGetIndex
 *
 * Get index of TER.
 *
 *  -> name		TER name.
 *
 * Returns index if found or dmMaxRecordIndex if not found.
 */
UInt16
TLGetIndex(Char *name)
{
	UInt16 idx;

	name+=6;
	for (idx=0; idx<terCnt; idx++) {
		if (StrCaselessCompare(name, terList[idx].name+6)==0)
			return idx;
	}

	return dmMaxRecordIndex;
}
