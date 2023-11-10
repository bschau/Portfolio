/*
**	fields.c
**
**	Code handle the field parsing.
*/
#include "include.h"

/* macros */
enum { FDone=0, FCategory, FDate, FDuration, FTask };

/* globals */
static struct {
	char *field;
	int len;
	int token;
} fields[]= {
	{ "category", 8, FCategory },
	{ "date", 4, FDate },
	{ "duration", 8, FDuration },
	{ "task", 4, FTask },
	{ NULL, 0, FDone },
};
static char *specStack;
static int specStackLen;
static void **jumpVec;

/*
**	GetField
**
**	Parse and store field in stack.
**
**	 -> t		Start of token.
**
**	Returns new token start.
*/
static char *
GetField(char *t)
{
	char *s;
	int idx, len;

	if (!*t)
		return NULL;

	for (s=t; *s && *s!=','; s++);
	if (*s==',')
		*s++='\x00';

	len=(int)strlen(t);
	for (idx=0; fields[idx].field; idx++) {
		if (len<=fields[idx].len && strncasecmp(t, fields[idx].field, len)==0) {
			specStack[specStackLen++]=fields[idx].token;
			return s;
		}
	}

	fprintf(stderr, "(GetField) Unknown field specifier %s\n", t);
	exit(1);
}

/*
**	FieldParse
**
**	Parse the output field specifier.
**
**	 -> spec	Field specifier.
*/
void
FieldParse(char *spec)
{
	int specLen=(int)strlen(spec);

	specStackLen=0;
	if ((specStack=(char *)malloc(specLen))==NULL) {
		fprintf(stderr, "(FieldParse) Cannot allocate stack\n");
		exit(1);
	}

	while ((spec=GetField(spec))!=NULL);
}

/*
**	FieldGetName
**
**	Get name X on stack.
**
**	 -> idx		Index on stack.
**
**	Return name or NULL if no more names.
*/
char *
FieldGetName(int idx)
{
	int type;

	if (idx<specStackLen) {
		type=specStack[idx];
		for (idx=0; fields[idx].field; idx++) {
			if (fields[idx].token==type)
				return fields[idx].field;
		}

		fprintf(stderr, "(FieldGetName) Garbage on stack\n");
		exit(1);
	}

	return NULL;
}

/*
**	FieldBuild
**
**	Build jump vector.
**
**	 -> cCmd	Command block.
*/
void
FieldBuild(ConvCMD *cCmd)
{
	int i;

	if ((jumpVec=malloc(sizeof(void *)*specStackLen))==NULL) {
		fprintf(stderr, "(FieldsBuild) Cannot build jump vector\n");
		exit(1);
	}

	for (i=0; i<specStackLen; i++) {
		switch (specStack[i]) {
		case FCategory:
			jumpVec[i]=cCmd->emitCategory;
			break;
		case FDate:
			jumpVec[i]=cCmd->emitDate;
			break;
		case FDuration:
			jumpVec[i]=cCmd->emitDuration;
			break;
		case FTask:
			jumpVec[i]=cCmd->emitTask;
			break;
		default:
			fprintf(stderr, "(FieldsBuild) Garbage on stack\n");
			exit(1);
		}
	}
}

/*
**	FieldRun
**
**	Emit the fields based on the jump vector.
**
**	 -> outH	Output Handle.
**	 -> prc		PRC.
**	 -> cCmd	Conv CMD block.
*/
void
FieldRun(FILE *outH, void *prc, ConvCMD *cCmd)
{
	void (*func)(FILE *, void *);
	int i;

	if (cCmd->comment)
		cCmd->comment(outH, prc);

	if (cCmd->sol)
		cCmd->sol(outH, prc);

	for (i=0; i<specStackLen; i++) {
		if (i) {
			if (cCmd->emitDelimiter)
				cCmd->emitDelimiter(outH, prc);
		}
		func=jumpVec[i];
		(*func)(outH, prc);
	}

	if (cCmd->eol)
		cCmd->eol(outH, prc);
}
