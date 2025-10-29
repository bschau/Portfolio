/*
**	oexcel.c
**
**	Code to handle the OExcel format.
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

	if (!srcLen) {
		fprintf(outH, "\"\"");
		return;
	}

	if (!whiteSpaceAsIs) {
		for (i=0; i<srcLen; i++) {
			if (isspace((int)*p))
				*p=' ';
	
			p++;
		}
	}

	p=src;
	for (i=0; i<srcLen; i++) {
		if (*p=='\n' || *p==delimiter) {
			quoteIt=1;
			break;
		}
		p++;
	}

	if (quoteIt) {
		fprintf(outH, "\"");
		CsvReset();
		p=src;
		for (i=0; i<srcLen; i++) {
			if (*p=='"') {
				CsvAdd(src, (int)(p-src));
				CsvAdd("\"\"", 2);
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
**	OExcelPrologue
*/
void
OExcelPrologue(FILE *f, void *prc)
{
	if (!useDelimiter)
		delimiter=(int)',';
}

/*
**	OExcelEOL
*/
void OExcelEOL(FILE *f, void *prc)
{
	fprintf(f, "%s", eol);
}

/*
**	OExcelCategory
*/
void OExcelCategory(FILE *f, void *prc)
{
	EscapeChars(f, prc, gCurCategory, (int)strlen(gCurCategory));
}

/*
**	OExcelDate
*/
void OExcelDate(FILE *f, void *prc)
{
	fprintf(f, "%s", gCurDate);
}

/*
**	OExcelDuration
*/
void OExcelDuration(FILE *f, void *prc)
{
	fprintf(f, "%s", gCurDuration);
}

/*
**	OExcelTask
*/
void OExcelTask(FILE *f, void *prc)
{
	EscapeChars(f, prc, gCurTask, gCurTaskLen);
}

/*
**	OExcelDelimiter
*/
void OExcelDelimiter(FILE *f, void *prc)
{
	fprintf(f, "%c", delimiter);
}
