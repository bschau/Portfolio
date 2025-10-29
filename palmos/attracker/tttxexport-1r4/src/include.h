#ifndef TTTXEXPORT
#define TTTXEXPORT

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "version.h"

#ifdef LINUX
#include <netinet/in.h>
#include <sys/errno.h>
#include <error.h>
#include <unistd.h>
#endif

#ifdef WIN32
#include "WinSock2.h"
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

/* macros */
#define EVER ;;
#define FMTDateLength 10
#define FMTDurationLength 5

/* structs */
typedef struct {
	void (*prologue)(FILE *, void *);
	void (*epilogue)(FILE *, void *);
	void (*comment)(FILE *, void *);
	void (*sol)(FILE *, void *);
	void (*eol)(FILE *, void *);
	void (*emitCategory)(FILE *, void *);
	void (*emitDate)(FILE *, void *);
	void (*emitDuration)(FILE *, void *);
	void (*emitTask)(FILE *, void *);
	void (*emitDelimiter)(FILE *, void *);
} ConvCMD;

/* comment.c */
void CommentParse(char *);
void CommentHeader(FILE *, void *);
void CommentComment(FILE *, void *, char *);

/* csvbuffer.c */
void CsvBufferSetSize(char *);
void CsvBufferAlloc(FILE *);
void CsvReset(void);
void CsvFlush(void);
void CsvAdd(char *, int);

/* datetime.c */
void DTSetDateFormat(char *);
void DTSetTimeFormat(char *);
char *DTDateFormat(unsigned short);
char *DTTimeFormat(unsigned short);

/* field.c */
void FieldParse(char *);
char *FieldGetName(int);
void FieldBuild(ConvCMD *);
void FieldRun(FILE *, void *, ConvCMD *);

#ifdef WIN32
/* getopt.c */
int getopt(int, char * const *, const char *);
#endif

/* odebug.c */
void ODebugPrologue(FILE *, void *);
/* void ODebugEpilogue(FILE *, void *); */
void ODebugComment(FILE *, void *);
/* void ODebugSOL(FILE *, void *); */
void ODebugEOL(FILE *, void *);
void ODebugCategory(FILE *, void *);
void ODebugDate(FILE *, void *);
void ODebugDuration(FILE *, void *);
void ODebugTask(FILE *, void *);
void ODebugDelimiter(FILE *, void *);

/* oexcel.c */
void OExcelPrologue(FILE *, void *);
void OExcelEOL(FILE *, void *);
void OExcelCategory(FILE *, void *);
void OExcelDate(FILE *, void *);
void OExcelDuration(FILE *, void *);
void OExcelTask(FILE *, void *);
void OExcelDelimiter(FILE *, void *);

/* ophp.c */
void OPhpPrologue(FILE *, void *);
void OPhpEpilogue(FILE *, void *);
void OPhpSOL(FILE *, void *);
void OPhpEOL(FILE *, void *);
void OPhpCategory(FILE *, void *);
void OPhpDate(FILE *, void *);
void OPhpDuration(FILE *, void *);
void OPhpTask(FILE *, void *);

/* ostdcsv.c */
void OStdCSVPrologue(FILE *, void *);
void OStdCSVComment(FILE *, void *);
void OStdCSVEOL(FILE *, void *);
void OStdCSVCategory(FILE *, void *);
void OStdCSVDate(FILE *, void *);
void OStdCSVDuration(FILE *, void *);
void OStdCSVTask(FILE *, void *);
void OStdCSVDelimiter(FILE *, void *);

/* prc.c */
void *PrcOpen(char *);
int PrcNumRecords(void *);
void *PrcGetFirstRecord(void *);
void *PrcGetNextRecord(void *);
int PrcGetRecIdx(void *);
int PrcGetRecLen(void *);
char *PrcGetCategoryName(void *);

/* globals */
#ifdef TTTXEXPORTMAIN
	int delimiter=',', useDelimiter=0, whiteSpaceAsIs=0;
#ifdef WIN32
	char *eol="\r\n";
#else
	char *eol="\n";
#endif
	char *gCurTask, *gCurCategory, *gCurDate, *gCurDuration;
	int gCurTaskLen;
#else
	extern int delimiter, useDelimiter, whiteSpaceAsIs;
	extern char *eol;
	extern char *gCurTask, *gCurCategory, *gCurDate, *gCurDuration;
	extern int gCurTaskLen;
#endif

/* all */
extern int errno;

#endif
