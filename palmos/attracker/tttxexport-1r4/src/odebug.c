/*
**	odebug.c
**
**	Code to handle the ODebug format.
*/
#include "include.h"

/*
**	ODebugPrologue
*/
void
ODebugPrologue(FILE *f, void *prc)
{
	CommentHeader(f, prc);
}

/*
**	ODebugComment
*/
void
ODebugComment(FILE *f, void *prc)
{
	char d[2];

	d[0]=(char)delimiter;
	d[1]='\x00';

	CommentComment(f, prc, &d[0]);
}

/*
**	ODebugEOL
*/
void ODebugEOL(FILE *f, void *prc)
{
	fprintf(f, "%s", eol);
}

/*
**	ODebugCategory
*/
void ODebugCategory(FILE *f, void *prc)
{
	fprintf(f, "%s", gCurCategory);
}

/*
**	ODebugDate
*/
void ODebugDate(FILE *f, void *prc)
{
	fprintf(f, "%s", gCurDate);
}

/*
**	ODebugDuration
*/
void ODebugDuration(FILE *f, void *prc)
{
	fprintf(f, "%s", gCurDuration);
}

/*
**	ODebugTask
*/
void ODebugTask(FILE *f, void *prc)
{
	fprintf(f, "%s", gCurTask);
}

/*
**	ODebugDelimiter
*/
void ODebugDelimiter(FILE *f, void *prc)
{
	fprintf(f, "%c", delimiter);
}
