/**
 * Context - LMakefile context.
 *
 * One context block is initialized per LMakefile.
 */
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Include.h"

/* globals */
extern char dirSepChar;

/**
 * Allocate and initialize a CTX.
 *
 * @param var Variables.
 * @param cwd Current Working Directory.
 * @param file LMakefile.
 * @return Context or NULL.
 */
CTX *
CTXNew(Var var, char *cwd, char *file)
{
	int cwdLen=strlen(cwd);
	CTX *c;

	if ((c=(CTX *)calloc(1, sizeof(CTX)+cwdLen+3))==NULL) {
		XError(NULL, "OOM while allocating Context");
	}

	c->var=var;

	c->cwd=((char *)c)+sizeof(CTX);
	memmove(c->cwd, cwd, cwdLen);

	if (file) {
		int seps=0;
		char *p;

		for (p=file; *p; p++) {
			if (*p=='/' || *p=='\\') {
				seps++;
			}
		}

		if (seps==0) {
			int len=strlen(file);
			char *t;

			if ((p=(char *)malloc(cwdLen+1+len+1))==NULL) {
				XError(NULL, "OOM while canonifying LMakefile path");
			}

			t=p;
			strcpy(t, cwd);
			t+=cwdLen;
			*t++=dirSepChar;
			strcpy(t, file);
			t[len]=0;
			c->lmakeFile=p;
		} else {
			c->lmakeFile=XRealPath(file);
		}
	} else {
		c->lmakeFile=NULL;
	}

	c->tb=SBNew(512);
	c->sb=SBNew(512);
	c->exp=SBNew(1024);
	c->args=SBNew(256);

	c->lineNo=1;
	c->ungetc=-1;

	return c;
}

/**
 * Open context.
 *
 * @param ctx Context.
 * @param src Source data or NULL to open file.
 */
void
CTXOpen(CTX *ctx, char *src)
{
	if (!src) {
		if ((ctx->fp=fopen(ctx->lmakeFile, "rb"))==NULL) {
			XError(NULL, "Failed to open LMakefile '%s'", ctx->lmakeFile);
		}
	} else {
		ctx->src=src;
	}
}

/**
 * Close context.
 *
 * @param ctx Context.
 */
void
CTXClose(CTX *ctx)
{
	if (ctx->fp) {
		fclose(ctx->fp);
		ctx->fp=NULL;
	}
}

/**
 * Destroys Context.
 *
 * @param ctx Context.
 */
void
CTXFree(CTX *ctx)
{
	if (ctx) {
		if (ctx->fp) {
			fclose(ctx->fp);
		}

		if (ctx->lmakeFile) {
			free(ctx->lmakeFile);
		}

		SBFree(ctx->exp);
		SBFree(ctx->tb);
		SBFree(ctx->sb);
		SBFree(ctx->args);

		free(ctx);
	}
}
