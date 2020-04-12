/**
 * Misc. utility functions.
 */
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Include.h"

/* globals */
extern char dirSepChar;

/**
 * Write error to stderr and exit.
 *
 * @param ctx Context (or NULL).
 * @param fmt Format string.
 * @param ... Varargs.
 */
void
XError(void *ctx, const char *fmt, ...)
{
	va_list ap;

	if (ctx) {
		fprintf(stderr, "(%s:%i) ", ((CTX *)ctx)->lmakeFile, ((CTX *)ctx)->lineNo);
	}

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, "\n");
	fflush(NULL);
	exit(1);
}

/**
 * Output string if verbose level is equal to strings verbose level.
 *
 * @param ctx Context.
 * @param lvl String level.
 * @param fmt Format string.
 * @param ... Varargs.
 */
void
XOut(CTX *ctx, int lvl, const char *fmt, ...)
{
	if (ctx->verbose>=lvl) {
		va_list ap;

		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);

		printf("\n");
		fflush(NULL);
	}
}

/**
 * Change directory to ..
 *
 * @param ctx Context (or NULL).
 * @param dir Directory.
 */
void
XChDir(CTX *ctx, char *dir)
{
	if (chdir(dir)) {
		XError(ctx, "Failed to change directory to %s", dir);
	}
}

/**
 * Get Current Working Directory.
 *
 * @param ctx Context (or NULL).
 * @return CWD.
 * @Note Caller must free returned cwd.
 */
char *
XGetCwd(CTX *ctx)
{
	char *cwd=getcwd(NULL, 0);

	if (!cwd) {
		XError(ctx, "Failed to get current working directory");
	}

	return cwd;
}

/**
 * Resolve . and .. and double // (or \\) in path.
 *
 * @param srcPath Source path.
 * @return Resolved path.
 */
char *
XRealPath(char *srcPath)
{
	char *src=UStrDup(NULL, srcPath), *last=NULL;
	char *cwd, *s;
	struct stat sBuf;

	s=src+strlen(src);
	while (--s>=src) {
		if (*s=='/' || *s=='\\') {
			*s=0;
		} else {
			break;
		}
	}

	for (s=src; *s && (*s!='/') && (*s!='\\'); s++);
	if (!*s) {
		return UStrDup(NULL, src);
	}

	if (stat(src, &sBuf)!=0) {
		XError(NULL, "'%s': invalid component in path", src);
	}

	if (!(S_ISDIR(sBuf.st_mode))) {
		last=src+strlen(src);
		while (--last>=src) {
			if (*last=='/' || *last=='\\') {
				break;
			}
		}

		if (last>src) {
			*last++=0;
		}
	}

	cwd=XGetCwd(NULL);

	XChDir(NULL, src);
	s=XGetCwd(NULL);

	if (last) {
		int sLen=strlen(s), lLen=strlen(last);
		char *p, *t;

		if ((p=malloc(sLen+1+lLen+1))==NULL) {
			XError(NULL, "OOM in XRealPath");
		}

		t=p;
		strcpy(t, s);
		free(s);

		t+=sLen;
		*t++=dirSepChar;
		strcpy(t, last);
		t+=lLen;
		*t=0;
		s=p;
	}

	XChDir(NULL, cwd);

	free(cwd);
	free(src);

	return s;
}

/**
 * Dump the arguments block to stdout.
 *
 * @param argc Argument count (or 0 to autodetect).
 * @param argv Argument block (or NULL).
 */
void
XDumpArgs(int argc, char **argv)
{
	int idx;

	printf("XDumpArgs: \n");
	if (argv==NULL) {
		return;
	}

	if (argc>0) {
		for (idx=0; idx<argc; idx++) {
			printf("argv[%02i] => %s\n", idx, argv[idx]);
		}
	} else {
		for (idx=0; argv[idx]; idx++) {
			printf("argv[%02i] => %s\n", idx, argv[idx]);
		}
	}
}

/**
 * Return token.
 *
 * @param ctx Context (or NULL).
 * @param src Source string.
 * @param len Where to store length of token.
 * @param dSrc Where to store updated source pointer (or NULL).
 * @returns token or NULL (if no token).
 */
char *
XGetToken(CTX *ctx, char *src, int *len, char **dSrc)
{
	char *token=NULL;
	int tLen=0;

	*len=0;

	for (; *src && isspace((int)*src); src++);
	if (!*src) {
		*dSrc=src;
		return NULL;
	}

	if ((*src=='\'') || (*src=='"')) {
		char delim=*src++;
		token=src;
		for (; *src && (*src!=delim); src++) {
			tLen++;
		}

		if (!*src) {
			XError(ctx, "Unterminated string");
		}

		src++;
	} else {
		token=src;
		for (; *src && (!isspace((int)*src)); src++) {
			tLen++;
		}
	}

	*len=tLen;
	*dSrc=src;

	return token;
}

/**
 * Form argv array from input.
 *
 * @param ctx Context.
 * @param appName argv[0] (or NULL).
 * @param argc Where to store argument count.
 * @param argv Where to store the arguments.
 */
void
XOptionPickUp(CTX *ctx, char *appName, int *argc, char ***argv)
{
	int tCnt, aIdx, idx;
	char *s;
	char **a;
	int term;

	SBReset(ctx->args);

	if (appName) {
		SBAppend(ctx->args, appName, strlen(appName)+1);
		aIdx=SBPosition(ctx->args);
		tCnt=1;
	} else {
		aIdx=0;
		tCnt=0;
	}

	for (EVER) {
		if (((idx=Peek(ctx))==TEOF) || (idx==TEOL) || (idx==TAnd) || (idx==TOr)) {
			break;
		}

		GetToken(ctx);
		s=Expand(ctx, SBToString(ctx->tb));
		SBAppend(ctx->args, s, strlen(s));
		SBAppendChar(ctx->args, ' ');
	}

	for (s=SBToString(ctx->args)+aIdx; *s; ) {
		for (; *s && isspace((int)*s); s++);
		if (!*s) {
			break;
		}

		if (*s=='"' || *s=='\'') {
			term=*s++;
			for (; *s && *s!=term; s++);
			s++;
		} else {
			for (; *s && (!isspace((int)*s)); s++);
		}

		tCnt++;
	}

	if ((a=(char **)malloc(sizeof(char *)*(tCnt+1)))==NULL) {
		XError(ctx, "OOM while allocating arguments array");
	}

	s=SBToString(ctx->args);
	if (aIdx) {
		a[0]=s;
		s+=aIdx;
		aIdx=1;
	}

	for (; *s; ) {
		for (; *s && isspace((int)*s); s++);
		if (!*s) {
			break;
		}

		if (*s=='"' || *s=='\'') {
			term=*s++;
			a[aIdx++]=s;
			for (; *s && *s!=term; s++);
		} else {
			a[aIdx++]=s;
			for (; *s && (!isspace((int)*s)); s++);
		}

		if (!*s) {
			break;
		}

		*s++=0;
	}

	a[aIdx]=NULL;

	*argv=a;
	*argc=tCnt;
}

/**
 * Free the arguments block.
 *
 * @param a Arguments.
 */
void
XOptionFree(char **a)
{
	if (a) {
		free(a);
	}
}
