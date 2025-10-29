#include "Include.h"

/* protos */
static Int16 QSortCompare(void *, void *, Int32);

/* globals */
static MemHandle pcListH;

/*
 * QSortCompare
 */
static Int16
QSortCompare(void *r1, void *r2, Int32 u)
{
	return StrCaselessCompare((Char *)r1, (Char *)r2);
}

/*
 * PCBuildList
 *
 * Build the Puzzle Collection list.
 */
void
PCBuildList(void)
{
	PCDestroyList();
	SysCreateDataBaseList('DATA', (UInt32)CRID, &pcCnt, &pcListH, false);
	if (pcListH) {
		pcList=MemHandleLock(pcListH);
		PCSort();
	}
}

/*
 * PCSort
 * 
 * Sort the Puzzle Collection list.
 */
void
PCSort(void)
{
	SysQSort(pcList, pcCnt, sizeof(SysDBListItemType), QSortCompare, 0);
}

/*
 * PCDestroyList
 *
 * Destroy the Puzzle Collection list.
 */
void
PCDestroyList(void)
{
	if (pcListH) {
		MemHandleFree(pcListH);
		pcListH=NULL;
		pcList=NULL;
		pcCnt=0;
	}
}

/*
 * PCGetIndex
 *
 * Get index of a Puzzle.
 *
 *  -> name		Puzzle name.
 *
 * Returns index if found or dmMaxRecordIndex if not found.
 */
UInt16
PCGetIndex(Char *name)
{
	UInt16 idx;

	for (idx=0; idx<pcCnt; idx++) {
		if (StrCaselessCompare(name, pcList[idx].name)==0)
			return idx;
	}

	return dmMaxRecordIndex;
}
