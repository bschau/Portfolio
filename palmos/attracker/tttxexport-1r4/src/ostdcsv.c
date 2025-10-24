/*
**	ostdcsv.c
**
**	Code to handle the OStdCSV format.
*/
#include "include.h"

/* protos */
static void EscapeChars(FILE *, void *, char *, int);

/*
**	EscapeChars
**
**	Print and possible escape characters.
**
**	 -> outH 	File.
**	 -> prc		PRC.
**	 -> src		Source string.
**	 -> srcLen	Length of source string.
*/
static void
EscapeChars(FILE *outH, void *prc, char *src, int srcLen)
{
	int quoteIt=0, i;
	char *p=src;

	for (i=0; i<srcLen; i++) {
		if (*p=='"' || *p==delimiter) {
			quoteIt=1;
			break;
		}
		p++;
	}

	if (!whiteSpaceAsIs) {
		p=src;
		for (i=0; i<srcLen; i++) {
			if (isspace((int)*p))
				*p=' ';

			p++;
		}
	}

	if (quoteIt) {
		fprintf(outH, "\"");
		CsvReset();
		p=src;
		for (i=0; i<srcLen; i++) {
			if (*p=='\\' || *p=='"') {
				CsvAdd(src, (int)(p-src));
				CsvAdd("\\", 1);
				CsvAdd(p, 1);
				src=++p;
			} else
				p++;
		}
			
		if (src!=p)
			CsvAdd(src, (int)(p-src));

		CsvFlush();
		fprintf(outH, "\"");
	} else
		fprintf(outH, "%s", src);
}

/*
**	OStdCSVPrologue
*/
void
OStdCSVPrologue(FILE *f, void *prc)
{
	if (!useDelimiter)
		delimiter=(int)',';

	CommentHeader(f, prc);
}

/*
**	OStdCSVComment
*/
void
OStdCSVComment(FILE *f, void *prc)
{
	char d[2];

	d[0]=(char)delimiter;
	d[1]='\x00';

	CommentComment(f, prc, &d[0]);
}

/*
**	OStdCSVEOL
*/
void OStdCSVEOL(FILE *f, void *prc)
{
	fprintf(f, "%s", eol);
}

/*
**	OStdCSVCategory
*/
void OStdCSVCategory(FILE *f, void *prc)
{
	EscapeChars(f, prc, gCurCategory, (int)strlen(gCurCategory));
}

/*
**	OStdCSVDate
*/
void OStdCSVDate(FILE *f, void *prc)
{
	fprintf(f, "%s", gCurDate);
}

/*
**	OStdCSVDuration
*/
void OStdCSVDuration(FILE *f, void *prc)
{
	fprintf(f, "%s", gCurDuration);
}

/*
**	OStdCSVTask
*/
void OStdCSVTask(FILE *f, void *prc)
{
	EscapeChars(f, prc, gCurTask, gCurTaskLen);
}

/*
**	OStdCSVDelimiter
*/
void OStdCSVDelimiter(FILE *f, void *prc)
{
	fprintf(f, "%c", delimiter);
}

