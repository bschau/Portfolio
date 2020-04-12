/**
 */
#define MAIN
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#ifdef Linux
#include <libgen.h>
#endif
#include "Include.h"

/* protos */
static void Usage(int);
static FILE *OpenOutput(char *, char *, char *);

/* globals */
extern int optIndex, errno;
extern char optError[];
extern char *optArg;

/**
 * Write usage information and exit with 'ec' status.
 *
 * @param ec Error code.
 */
static void
Usage(int ec)
{
	FILE *f;

	if (ec) {
		f=stderr;
	} else {
		f=stdout;
	}

	fprintf(f, "lme from ltools-%s\n", VERSION);
	fprintf(f, "Usage: lme [OPTIONS] file\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -S tag        Start tag - default is $(\n");
	fprintf(f, "  -E tag        End tag - default is )\n");
	fprintf(f, "  -h            This page\n");
	fprintf(f, "  -f file       Output to file\n");
	fprintf(f, "  -o dir        Output file to 'dir'\n");
	fprintf(f, "  -p file       Properties file (can be given multiple times)\n");
	fprintf(f, "  -v            Raise verbosity\n");
	exit(ec);
}

/**
 * Main.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 if successful, !0 otherwise.
 */
int
main(int argc, char *argv[])
{
	char *oDir=NULL, *oFile=NULL;
	char *iFileMem;
	int opt;

	if ((vars=HashNew(8))==NULL) {
		XError(NULL, "Failed to allocate variables block");
	}

	OptionInit();
	while ((opt=OptionNext(argc, argv, "E:S:hf:o:p:v"))!=-1) {
		switch (opt) {
		case 'E':
			endTag=optArg;
			break;

		case 'S':
			startTag=optArg;
			break;

		case 'h':
			Usage(0);

		case 'f':
			oFile=optArg;
			break;

		case 'o':
			oDir=optArg;
			break;

		case 'p':
			PropertiesRead(optArg);
			break;

		case 'v':
			if (verbose<XODebug) {
				verbose++;
			}
			break;

		default:
			fprintf(stderr, "%s\n", optError);
			Usage(1);
		}
	}

	if (argc-optIndex!=1) {
		Usage(1);
	}

	inFile=argv[optIndex];
	XOut(XODebug, "Input file is: '%s'", inFile);

	iFileMem=XReadFile(inFile);

	outFile=OpenOutput(inFile, oDir, oFile);

	startTagLen=strlen(startTag);
	endTagLen=strlen(endTag);
	XOut(XODebug, "Start tag is: '%s'", startTag);
	XOut(XODebug, "End tag is: '%s'", endTag);
	Parse(iFileMem);

	fclose(outFile);

	XOut(XODebug, "Done");
	exit(0);
}

/**
 * Open output file.
 *
 * @param inF In file.
 * @param oD Output dir.
 * @param oF Output file.
 * @param returns open file.
 */
static FILE *
OpenOutput(char *inF, char *oD, char *oF)
{
	char *oFile=NULL;
	FILE *f;

	if (oD) {
		int oDLen=strlen(oD), bLen;
		char *bName;

		if (oF) {
			bName=basename(oF);
		} else {
			bName=basename(inF);
		}

		bLen=strlen(bName);
		oFile=UAlloc(NULL, oDLen+1+bLen);
		memmove(oFile, oD, oDLen);
		oFile[oDLen]=dirSepChar;
		memmove(oFile+oDLen+1, bName, bLen);
	} else if (oF) {
		oFile=oF;
	} else {
		oFile=basename(inF);
	}


	if ((f=fopen(oFile, "wb+"))==NULL) {
		XError(NULL, "%s:%s", oFile, strerror(errno));
	}

	return f;
}
