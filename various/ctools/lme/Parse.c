/**
 * Reading and parsing of the target file.
 */
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Include.h"

/* protos */
static void Run(void);
static void Expand(void);
static void DeepExpand(char *);
static void GetVariable(char *);
static void GetCommand(char *);
static void GetRun(char *);
static void GetFile(char *);
static void FormArgvArray(char *, int *, char ***);
static int ScanPastToken(char *, int *, int *, int *);

/* globals */
static int lineNo=0;
static char ticksBuffer[256];
static StringBuilder sb, sbTmp, sbArgs;
static char *mem;
extern int errno;

/**
 * Read and parse the target file.
 *
 * @param m File in memory.
 */
void Parse(char *m)
{
	sb=SBNew(512);
	sbTmp=SBNew(512);
	sbArgs=SBNew(512);

	mem=m;

	Run();
}

/**
 * Gut of the parser.
 */
static void
Run(void)
{
	char *p;

	while (*mem) {
		/* find end */
		for (p=mem; *p && (*p!='\n'); p++);
		lineNo++;
		p++;
		SBReset(sb);
		SBAppend(sb, mem, p-mem);
		mem=p;

		Expand();

		if (fwrite(SBToString(sb), SBLength(sb), 1, outFile)!=1) {
			XError(NULL, "Failed to write to output file");
		}
	}
}

/**
 * Run expander on the string in the string builder.
 */
static void
Expand(void)
{
	char *str=SBToString(sb), *p, *end;

	p=str+SBLength(sb)-startTagLen;

	while (p>=str) {
		if (memcmp(p, startTag, startTagLen)==0) {
			if ((end=strstr(p+startTagLen, endTag))!=NULL) {
				SBReset(sbTmp);
				SBAppend(sbTmp, str, (p-str));

				*end=0;
				DeepExpand(p+startTagLen);

				SBAppend(sbTmp, end+endTagLen, str+SBLength(sb)-end-endTagLen);

				SBCopyTo(sbTmp, sb);

				str=SBToString(sb);
				p=str+SBLength(sb)-startTagLen;
			}
		}
		p--;
	}
}

/**
 * Expand a 'command'.
 *
 * @param cmd Command start.
 */
static void
DeepExpand(char *cmd)
{
	char *s;

	switch (*cmd) {
	case '@':
		GetCommand(cmd+1);
		break;
	case '!':
		GetRun(cmd+1);
		break;
	case '=':
		GetFile(cmd+1);
		break;
	case '$':
		if ((s=getenv(cmd+1))!=NULL) {
			SBAppend(sbTmp, s, strlen(s));
		} else {
			XError(NULL, "System variable '%s' not found");
		}
		break;
	default:
		GetVariable(cmd);
		break;
	}
}

/**
 * Expand variable to sbTmp.
 *
 * @param var Variable.
 */
static void
GetVariable(char *var)
{
	void *data;
	int size;

	if ((data=HashGet(vars, var, &size))==NULL) {
		XError(NULL, "Variable '%s' not found");
	}

	SBAppend(sbTmp, (char *)data, size);
}

/**
 * Expand command to sbTmp.
 * 
 * @param var Command.
 */
static void
GetCommand(char *cmd)
{
	char *s=NULL;
	char **argv=NULL;
	struct stat sBuf;
	int argc;
	time_t now;

	FormArgvArray(cmd, &argc, &argv);

	if ((argc==0) || (argv==NULL) || (argv[0]==NULL)) {
		XError(NULL, "No command detected");
	}

	if (strcmp(argv[0], "os")==0) {
		s=OS;
	} else if (strcmp(argv[0], "userid")==0) {
		s=OSUserId();
	} else if (strcmp(argv[0], "seconds")==0) {
		now=time(NULL);
		sprintf(ticksBuffer, "%lu", now);
		s=ticksBuffer;
	} else if (strcmp(argv[0], "ctime")==0) {
		now=time(NULL);
		s=ctime(&now);
		s[strlen(s)-1]=0;
	} else if (strncmp(argv[0], "file", 4)==0) {
		if (argc<2) {
			XError(NULL, "Invalid number of arguments to %s", argv[0]);
		}

		if (strcmp(argv[1], "this")==0) {
			argv[1]=inFile;
		}

		if (stat(argv[1], &sBuf)!=0) {
			XError(NULL, "Cannot stat %s (%s)\n", argv[1], strerror(errno));
		}

		s=argv[0]+4;
		if (strcmp(s, "size")==0) {
			sprintf(ticksBuffer, "%lu", sBuf.st_size);
			s=ticksBuffer;
		} else if (strncmp(s+1, "time", 4)==0) {
			struct tm *mt;

			switch (*s) {
			case 'a':
				now=sBuf.st_atime;
				break;
			case 'c':
				now=sBuf.st_ctime;
				break;
			case 'm':
				now=sBuf.st_mtime;
				break;
			default:
				XError(NULL, "No such command '%s'", argv[0]);
			}

			if (argv[2]) {
				size_t si;

				if (*(s+5)=='g') {
					mt=gmtime(&now);
				} else {
					mt=localtime(&now);
				}

				si=strftime(ticksBuffer, sizeof(ticksBuffer)-1, argv[2], mt);
				ticksBuffer[si]=0;
				s=ticksBuffer;
			} else {
				s=ctime(&now);
				s[strlen(s)-1]=0;
			}
		}
	}

	free(argv);

	if (s) {
		SBAppend(sbTmp, s, strlen(s));
	} else {
		XError(NULL, "No such command '%s'", cmd);
	}
}

/**
 * Expand output of external command to sbTmp.
 * 
 * @param var Command.
 */
static void
GetRun(char *cmd)
{
	int retCode;
	FILE *fp;

	if ((fp=popen(cmd, "r"))==NULL) {
		XError(NULL, "Failed to run '%s'", cmd);
	}

	while (fgets(ticksBuffer, sizeof(ticksBuffer), fp)) {
		SBAppend(sbTmp, ticksBuffer, strlen(ticksBuffer));
	}

	retCode=pclose(fp);
}

/**
 * Expand content of file to sbTmp.
 * 
 * @param file Command.
 */
static void
GetFile(char *file)
{
	char *mem=XReadFile(file);

	SBAppend(sbTmp, mem, strlen(mem));
}

/**
 * Form argv array from input.
 *
 * @param cmd block.
 * @param argc Where to store argument count.
 * @param argv Where to store the arguments.
 */
static void
FormArgvArray(char *cmd, int *argc, char ***argv)
{
	int aCnt=0, skipBlanks, quotes, tokenLen, idx;
	char *p=cmd;
	char **a;

	/* count arguments */
	while (*p) {
		if (!ScanPastToken(p, &skipBlanks, &quotes, &tokenLen)) {
			break;
		}

		aCnt++;

		p+=skipBlanks+tokenLen;
		if (quotes) {
			p+=2;
		}
	}

	/* allocate argv array */
	*argc=aCnt;
	if (!aCnt) {
		*argv=NULL;
		return;
	}

	if ((a=(char **)malloc(sizeof(char *)*(aCnt+1)))==NULL) {
		XError(NULL, "OOM while allocating arguments array");
	}

	idx=0;
	while (*cmd) {
		if (!ScanPastToken(cmd, &skipBlanks, &quotes, &tokenLen)) {
			break;
		}

		cmd+=skipBlanks;
		if (quotes) {
			cmd++;
		}

		a[idx++]=cmd;
		cmd+=tokenLen;
		if (*cmd) {
			*cmd++=0;
		}
	}

	a[idx]=NULL;
	*argv=a;
}

/**
 * Scan past the next token.
 *
 * @param src Source string.
 * @param skipBlanks Where to store number of blanks to skip.
 * @param quotes Did we see any quotes.
 * @param tokenLen Where to store length of token.
 * @return !0 if a token was found, 0 if no more tokens.
 */
static int
ScanPastToken(char *src, int *skipBlanks, int *quotes, int *tokenLen)
{
	int len;

	*quotes=0;
	*tokenLen=0;

	for (len=0; *src && isspace((int)*src); src++, len++);
	*skipBlanks=len;

	if (!*src) {
		return 0;
	}

	len=0;
	if ((*src=='"') || (*src=='\'')) {
		char term=*src++;

		*quotes=1;

		for (; *src && (*src!=term); src++, len++);
	} else {
		for (; *src && (!isspace((int)*src)); src++, len++);
	}

	*tokenLen=len;
	return 1;
}
