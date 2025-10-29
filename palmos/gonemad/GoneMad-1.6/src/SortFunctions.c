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

	return StrCaselessCompare(t1, t2);
}
