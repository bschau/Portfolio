/**
 * This is the main entry class. Usage of lcdoc:
 *
 * <PRE>
 * lcdoc [OPTIONS] files<BR>
 * <BR>
 * [OPTIONS]<BR>
 *     -S style      Style sheet<BR>
 *     -h            This page<BR>
 *     -o dir        Output files to 'dir'<BR>
 *     -s            Add static declarations to output<BR>
 *     -v            Raise verbosity<BR>
 * </PRE>
 */
#define MAIN
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Include.h"

/* protos */
static void Usage(int);
static void DumpList(char *, struct Node *);

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

	fprintf(f, "lcdoc from ltools-%s\n", VERSION);
	fprintf(f, "Usage: lcdoc [OPTIONS] files\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -S style      Style sheet\n");
	fprintf(f, "  -h            This page\n");
	fprintf(f, "  -o dir        Output files to 'dir'\n");
	fprintf(f, "  -s            Add static declarations to output\n");
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
	char *oDir="doc", *style=NULL;
	struct stat sBuf;
	int opt;

	OptionInit();
	while ((opt=OptionNext(argc, argv, "S:ho:sv"))!=-1) {
		switch (opt) {
		case 'S':
			style=optArg;
			break;

		case 'h':
			Usage(0);

		case 'o':
			oDir=optArg;
			break;

		case 's':
			addStatics=1;
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

	if (argc-optIndex<1) {
		Usage(1);
	}

	UInit(XError);
	if (style) {
		FILE *fp;

		XOut(XODebug, "Loading external style sheet '%s'", style);

		if (stat(style, &sBuf)) {
			XError("Failed to stat '%s'", style);
		}

		styleSheet=UAlloc(NULL, sBuf.st_size+2);
		if ((fp=fopen(style, "rb"))==NULL) {
			XError("Failed to open '%s'", style);
		}

		if (fread(styleSheet, sBuf.st_size, 1, fp)!=1) {
			XError("Failed to read '%s'", style);
		}

		fclose(fp);
		styleSheet=UTrim(styleSheet);
	}

	ParseOpen();
	XOut(XOInfo, "Loading files");
	for (; optIndex<argc; optIndex++) {
		ParseNew(argv[optIndex]);
		Parse();
		ParseFree();
	}
	
	if ((opt=stat(oDir, &sBuf))) {
		if (errno!=ENOENT) {
			XError("Failed to stat '%s' (%i)", oDir, errno);
		} else {
#ifdef Linux
			if (mkdir(oDir, 0777)) {
#else
			if (mkdir(oDir)) {
#endif
				XError("Failed to create output directory '%s'", oDir);
			}
		}
	} else {
		if (!(S_ISDIR(sBuf.st_mode))) {
			XError("%s exists but is not a directory", oDir);
		}
	}

	if (chdir(oDir)) {
		XError("Failed to change directory to '%s'", oDir);
	}

	XOut(XOInfo, "Generating pages");
//	DumpList("Functions", fList);
	Output();

	XOut(XOInfo, "Generating index");
	Index();

	ParseClose();

	if (style) {
		free(styleSheet);
	}

	XOut(XOInfo, "Done");
	exit(0);
}

/**-
 * Dump list beginning with this node.
 *
 * @param t Title.
 * @param n Node.
 */
static void
DumpList(char *t, struct Node *n)
{
	printf("%s ====>", t);

	if (n) {
		printf("\n");
		for (; n; n=n->next) {
			printf("    %s\n", n->key);
		}
	} else {
		printf(" none\n");
	}
}
