/*
**	ophp.c
**
**	Code to handle the OPhp format.
*/
#include "include.h"

/*
**	OPhpPrologue
*/
void
OPhpPrologue(FILE *f, void *prc)
{
	fprintf(f, "a:%i:{", PrcNumRecords(prc));
}

/*
**	OPhpEpilogue
*/
void
OPhpEpilogue(FILE *f, void *prc)
{
	fprintf(f, "}%s", eol);
}

/*
**	OPhpSOL
*/
void OPhpSOL(FILE *f, void *prc)
{
	fprintf(f, "i:%i;a:4:{", PrcGetRecIdx(prc));
}

/*
**	OPhpEOL
*/
void OPhpEOL(FILE *f, void *prc)
{
	fprintf(f, "}");
}

/*
**	OPhpCategory
*/
void OPhpCategory(FILE *f, void *prc)
{
	fprintf(f, "s:8:\"category\";s:%i:\"%s\";", strlen(gCurCategory), gCurCategory);
}

/*
**	OPhpDate
*/
void OPhpDate(FILE *f, void *prc)
{
	fprintf(f, "s:4:\"date\";s:%i:\"%s\";", FMTDateLength, gCurDate);
}

/*
**	OPhpDuration
*/
void OPhpDuration(FILE *f, void *prc)
{
	fprintf(f, "s:8:\"duration\";s:%i:\"%s\";", FMTDurationLength, gCurDuration);
}

/*
**	OPhpTask
*/
void OPhpTask(FILE *f, void *prc)
{
	fprintf(f, "s:4:\"task\";s:%i:\"%s\";", gCurTaskLen, gCurTask);
}
