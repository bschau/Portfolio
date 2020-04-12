/**
 * Main.
 */
#define MAIN
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Include.h"

/* protos */
static void Version(FILE *);
static void Usage(int);
static int LMakefileOpenable(char *);
static char **GetArguments(char *, char **, int *);

/* globals */
extern int optIndex;
extern char optError[];
extern char *optArg;

/**
 * Print version banner.
 */
static void
Version(FILE *d)
{
	fprintf(d, "lmake from ltools-%s\n", VERSION);
}

/**
 * Write usage information.
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

	Version(f);
	fprintf(f, "Usage: lmake [OPTIONS] [procedure1 ... procedureX]\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -C dir        Change to directory before processing files.\n");
	fprintf(f, "  -D var[=val]  Define variable. If val is omitted, val is set to 1.\n");
	fprintf(f, "  -U var        Undefine variable.\n");
	fprintf(f, "  -V            Print version string and exit.\n");
	fprintf(f, "  -d            Dry run, don't do anything.\n");
	fprintf(f, "  -f file       Use file as LMakefile.\n");
	fprintf(f, "  -h            This page.\n");
	fprintf(f, "  -v            Raise verbosity.\n");
	fprintf(f, "\nIf [procedure] is omitted the 'all' procedure is called.\n");
	exit(ec);
}

/**
 * Test if the given path is an openable LMakefile.
 *
 * @param path Full path to LMakefile.
 * @return !0 if file can be opened, 0 otherwise.
 */
static int
LMakefileOpenable(char *path)
{
	FILE *f=fopen(path, "rb");

	if (f==NULL) {
		return 0;
	}

	fclose(f);
	return 1;
}

/**
 * Perform system initialization of the globals.
 */
int
main(int argc, char *argv[])
{
	int sArgc=1, idx;
	char **sArgv;

	UInit(XError);
	globals=VarNew(NULL);
	VarInitialize(globals);

	if ((sArgv=calloc(argc+2, sizeof(char *)))==NULL) {
		XError(NULL, "OOM while allocating arguments array");
	}

	sArgv[0]=argv[0];

	for (idx=1; idx<argc; idx++) {
		if (argv[idx][0]=='-') {
			if (argv[idx][1]=='-') {
				for (; idx<argc; idx++) {
					sArgv[sArgc++]=argv[idx++];
				}

				break;
			}

			switch (argv[idx][1]) {
			case 'd':
				dryRun=1;
				break;
			case 'h':
				Usage(0);
			case 'V':
				Version(stdout);
				exit(0);
			default:
				sArgv[sArgc++]=argv[idx];
				break;
			}

		} else
			sArgv[sArgc++]=argv[idx];
	}

	sArgv[sArgc]=NULL;

	Run(globals, sArgc, sArgv, 0);
	exit(0);
}

/**
 * Perform actual parsing of LMakefiles and run procedures.
 *
 * @param var Parent/globals.
 * @param argc Argument count.
 * @param argv Argument list.
 * @param resetVerbose Initial verbose value.
 */
void
Run(Var var, int argc, char *argv[], int resetVerbose)
{
	char *lmakeFile=NULL, *cwd=NULL, *prevDir=NULL;
	int verbose=resetVerbose;
	VARDesc vd;
	CTX *ctx;
	int opt;
	char *p;

	OptionInit();
	while ((opt=OptionNext(argc, argv, "C:D:U:f:v"))!=-1) {
		switch (opt) {
		case 'C':
			cwd=optArg;
			break;

		case 'D':
			if ((p=strchr(optArg, '='))==NULL) {
				VarSet(var, optArg, "1", 2, 0, NULL);
			} else {
				*p=0;
				if (strlen(p+1)>0) {
					VarSet(var, optArg, p+1, strlen(p+1)+1, 0, NULL);
				} else {
					VarDelete(var, optArg);
				}

				*p='=';
			}
			break;

		case 'U':
			if ((p=strchr(optArg, '='))!=NULL) {
				*p=0;
			}

			VarDelete(var, optArg);
			if (p) {
				*p='=';
			}
			break;
			
		case 'f':
			lmakeFile=optArg;
			break;

		case 'v':
			if (verbose<3) {
				verbose++;
			}
			break;

		default:
			fprintf(stderr, "%s\n", optError);
			Usage(1);
		}
	}

	if (cwd) {
		char *dir;

		prevDir=XGetCwd(NULL);
		VarSetSystem(var, "PWD", prevDir, strlen(prevDir)+1, 0, NULL);

		XChDir(NULL, cwd);
		dir=XGetCwd(NULL);
		VarSetSystem(var, "CWD", dir, strlen(dir)+1, 0, NULL);
		free(dir);
	}

	if (!VarGet(var, "CWD", &vd)) {
		XError(NULL, "Failed to retrieve current directory"); 
	}

	cwd=strdup(vd.data);
	if (lmakeFile) {
		if (!LMakefileOpenable(lmakeFile)) {
			XError(NULL, "Failed to open '%s' in %s", lmakeFile, cwd);
		}
	} else {
		if (!LMakefileOpenable("LMakefile")) {
			if (!LMakefileOpenable("lmakefile")) {
				XError(NULL, "No LMakefile or lmakefile in %s", cwd);
			} else {
				lmakeFile="lmakefile";
			}
		} else {
			lmakeFile="LMakefile";
		}
	}

	ctx=CTXNew(var, cwd, lmakeFile);
	CTXOpen(ctx, NULL);
	LMParse(ctx);
	CTXClose(ctx);
	CTXFree(ctx);

	if (optIndex<argc) {
		char *procedure;
		int vargc;
		char **varg;

		for (; optIndex<argc; optIndex++) {
			varg=GetArguments(argv[optIndex], &procedure, &vargc);
			LMake(var, cwd, procedure, verbose, vargc, varg);
			if (varg) {
				free(varg);
			}
		}
	} else {
		LMake(var, cwd, "all", verbose, 0, NULL);
	}

	free(cwd);

	if (prevDir) {
		XChDir(NULL, prevDir);

		if (!VarGet(var, "CWD", &vd)) {
			XError(NULL, "CWD not set");
		}

		VarSetSystem(var, "PWD", vd.data, vd.dLen, 0, NULL);
		VarSetSystem(var, "CWD", prevDir, strlen(prevDir)+1, 0, NULL);
	}
}

/**
 * Parse arguments out of string.
 *
 * @param src Source string.
 * @param proc Where to store procedure name.
 * @param argc Where to store argument count.
 * @return argv or NULL if no arguments.
 */
static char **
GetArguments(char *src, char **proc, int *argc)
{
	int aIdx=0, tCnt=0;
	char delim;
	char *p;
	char **a;

	*proc=NULL;
	*argc=0;

	/* pickup procedure name */
	if ((*src=='\'') || (*src=='"')) {
		delim=*src++;
		for (p=src; *p && (*p!=delim); p++);
		if (!*p) {
			XError(NULL, "Missing terminator %c in argument list (%s)", delim, src-1);
		}
	}

	*proc=src;
	for (; *src && (*src!=':'); src++);

	if (!*src) {
		*proc=UTrim(*proc);
		return NULL;
	}

	*src++=0;
	*proc=UTrim(*proc);

	/* pick up arguments */
	p=src;
	while (*p) {
		for (; *p && isspace((int)*p); p++);
		if ((*p=='"') || (*p=='\'')) {
			for (delim=*p++; *p && (delim!=*p); p++);
			if (*p!=delim) {
				XError(NULL, "Missing %c in argument list (%s)", delim, src);
			}	

			p++;
		} else {
			for (; *p && (!isspace((int)*p)); p++);
		}

		tCnt++;
	}

	if ((a=(char **)malloc(sizeof(char *)*(tCnt+1)))==NULL) {
		XError(NULL, "OOM while allocating arguments array");
	}

	*argc=tCnt;
	p=src;
	while (tCnt) {
		for (; *p && isspace((int)*p); p++);
		if ((*p=='"') || (*p=='\'')) {
			delim=*p++;
			a[aIdx]=p;
			for (; *p && (delim!=*p); p++);
		} else {
			a[aIdx]=p;
			for (; *p && (!isspace((int)*p)); p++);
		}
		*p++=0;
		aIdx++;
		tCnt--;
	}

	a[aIdx]=NULL;
	return a;
}
