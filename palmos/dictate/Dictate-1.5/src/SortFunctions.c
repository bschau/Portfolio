/*
 * SortFunctions.c
 */
#include "Include.h"

/*
 * SFString
 */
Int16
SFString(void *r1, void *r2, Int16 u1, SortRecordInfoPtr u2, SortRecordInfoPtr u3, MemHandle u4)
{
	Char *t1=(Char *)r1, *t2=(Char *)r2;
	UInt16 d1, d2;

	return TxtGlueCaselessCompare(t1, StrLen(t1), &d1, t2, StrLen(t2), &d2);
}

/*
 * SFScore
 */
Int16
SFScore(void *r1, void *r2, Int16 u1, SortRecordInfoPtr u2, SortRecordInfoPtr u3, MemHandle u4)
{
	Char *t1=((Score *)r1)->name, *t2=((Score *)r2)->name;
	UInt16 d1, d2;

	return TxtGlueCaselessCompare(t1, StrLen(t1), &d1, t2, StrLen(t2), &d2);
}

