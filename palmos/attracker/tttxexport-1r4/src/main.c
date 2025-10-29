/*
**	main.c
**
**	Main entry point.
*/
#define TTTXEXPORTMAIN
#include "include.h"

/* protos */
static void Usage(int);
static void SetDelimiter(char *);

/* globals */
/* prologue, epilogue, comment, sol, eol, cat, date, dur, task, delim */
static ConvCMD oDebug={ ODebugPrologue, NULL, ODebugComment, NULL, ODebugEOL, ODebugCategory, ODebugDate, ODebugDuration, ODebugTask, ODebugDelimiter };
static ConvCMD oExcel={ OExcelPrologue, NULL, NULL, NULL, OExcelEOL, OExcelCategory, OExcelDate, OExcelDuration, OExcelTask, OExcelDelimiter };
static ConvCMD oPhp={ OPhpPrologue, OPhpEpilogue, NULL, OPhpSOL, OPhpEOL, OPhpCategory, OPhpDate, OPhpDuration, OPhpTask, NULL };
static ConvCMD oStdCSV={ OStdCSVPrologue, NULL, OStdCSVComment, NULL, OStdCSVEOL, OStdCSVCategory, OStdCSVDate, OStdCSVDuration, OStdCSVTask, OStdCSVDelimiter };
static ConvCMD *cCmd=&oExcel;

#ifdef WIN32
extern char *optarg;
extern int optind;
#endif

/*
**	Usage
**
**	Write Usage and exit with ec.
**
**	 -> eC		Error code.
*/
static void
Usage(int eC)
{
	FILE *f;

	if (eC)
		f=stderr;
	else
		f=stdout;

	fprintf(f, "tttxexport v%s by Brian Schau <mailto:brian@schau.com>\n\n", VERSION);
	fprintf(f, "Usage: tttxexport [OPTIONS] .pdb-file\n\n");
	fprintf(f, "OPTIONS:\n\n");
	fprintf(f, " -b size           Set buffer size\n");
	fprintf(f, " -c                Emit comments\n");
	fprintf(f, " -C lines,prefix   Comment specifiers\n");
	fprintf(f, " -d delimiter      Delimiter character\n");
	fprintf(f, " -D format         Date format\n");
	fprintf(f, " -f fields         Output fields\n");
	fprintf(f, " -h                Help text - this text\n");
	fprintf(f, " -L type           Linefeed type\n");
	fprintf(f, " -o file           Output file\n");
	fprintf(f, " -O type           Output type\n");
	fprintf(f, " -T format         Time format\n");
	fprintf(f, " -W                Do not convert whitespaces\n");
	exit(eC);
}

/*
**	SetDelimiter
**
**	Parse and set new delimiter.
**
**	 -> dS		Delimiter string.
*/
static void
SetDelimiter(char *dS)
{
	int dLen=(int)strlen(dS);

	if (dLen==1) {
		delimiter=(int)*dS;
		return;
	}

	if (*dS=='\\') {
		dS++;
		switch (*dS) {
		case 't':
			delimiter=9;
			break;
		case 'n':
			delimiter=10;
			break;
		case 'r':
			delimiter=13;
			break;
		case '0':
			delimiter=0;
			break;
		default:
			fprintf(stderr, "(SetDelimiter) Unknown backspace delimiter %c\n", *dS);
			exit(1);
		}

		return;
	}

	if (*dS=='0' && *(dS+1)=='x') {
		if (dLen==4) {
			sscanf(dS, "%X", &delimiter);
			return;
		}

		fprintf(stderr, "(SetDelimiter) Hex delimiter not in form 0xXX\n");
		exit(1);	
	}

	fprintf(stderr, "(Setdelimiter) Invalid delimiter %s\n", dS);
	exit(1);
}

/*
**	main
*/
int
main(int argc, char *argv[])
{
	char *fields=NULL;
	char *outFile=NULL;
	FILE *outH=NULL;
	int opt;
	void *p, *v;
	unsigned short *rC;

	if ((fields=strdup("category,date,duration,task"))==NULL) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}
	
	while ((opt=getopt(argc, argv, "b:cC:dD:f:hL:o:O:T:W"))!=-1) {
		switch (opt) {
		case 'b':
			CsvBufferSetSize(optarg);
			break;
		case 'c':
			CommentParse(strdup("25,#"));
			break;
		case 'C':
			CommentParse(optarg);
			break;
		case 'd':
			SetDelimiter(optarg);
			useDelimiter=1;
			break;
		case 'D':
			DTSetDateFormat(optarg);
			break;
		case 'f':
			fields=optarg;
			break;
		case 'h':
			Usage(0);
		case 'L':
			switch (*optarg) {
			case 'm':
			case 'M':
				eol="\r";
				break;
			case 'u':
			case 'U':
				eol="\n";
				break;
			case 'w':
			case 'W':
				eol="\r\n";
				break;
			default:
				fprintf(stderr, "(main) Unknown eol specifier %s\n", optarg);
				exit(1);
			}
			break;
		case 'o':
			outFile=optarg;
			break;
		case 'O':
			if (strcasecmp(optarg, "debug")==0)
				cCmd=&oDebug;
			else if (strcasecmp(optarg, "excel")==0)
				cCmd=&oExcel;
			else if (strcasecmp(optarg, "php")==0)
				cCmd=&oPhp;
			else if (strcasecmp(optarg, "stdcsv")==0)
				cCmd=&oStdCSV;
			else {
				fprintf(stderr, "(main) Output type unknown\n");
				Usage(1);
			}
			break;
		case 'T':
			DTSetTimeFormat(optarg);
			break;
		case 'W':
			whiteSpaceAsIs=1;
			break;
		default:
			Usage(1);
		}
	}

	if ((argc-optind)!=1)
		Usage(1);

	FieldParse(fields);
	FieldBuild(cCmd);

	if (!outFile)
		outFile="tttxexport.out";
	else if (*outFile=='-' && strlen(outFile)==1)
		outFile="/dev/stdout";

	p=PrcOpen(argv[optind]);
#ifdef WIN32
	if ((outH=fopen(outFile, "w+b"))==NULL) {
#else
	if ((outH=fopen(outFile, "w+"))==NULL) {
#endif
		fprintf(stderr, "(main) Cannot open output file %s for writing (%i)\n", outFile, errno);
		exit(1);
	}

	CsvBufferAlloc(outH);
	if (cCmd->prologue)
		cCmd->prologue(outH, p);

	v=PrcGetFirstRecord(p);
	while (v) {
		rC=(unsigned short *)v;
		gCurDate=DTDateFormat(ntohs(*rC));
		gCurDuration=DTTimeFormat(ntohs(*(rC+1)));
		gCurTask=(char *)(rC+2);
		/* -1 below is the trailing zero - we don't want that .. */
		gCurTaskLen=PrcGetRecLen(p)-(2*sizeof(unsigned short))-1;
		gCurCategory=PrcGetCategoryName(p);
		FieldRun(outH, p, cCmd);
		free(v);
		v=PrcGetNextRecord(p);
	}

	if (cCmd->epilogue)
		cCmd->epilogue(outH, p);

	fflush(outH);
	fclose(outH);
	exit(0);
}
