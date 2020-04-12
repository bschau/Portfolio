/**
 * if-construct handler.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Include.h"

/* protos */
static void Stat2Files(CTX *, char *, char *, time_t *, time_t *);
static void FindOptElse(CTX *, char *);
static int TIfDir(CTX *, char *);
static int TIfExists(CTX *, char *);
static int TIfFile(CTX *, char *);
static int TIfNonZeroFile(CTX *, char *);
static int TIfFileNewerThan(CTX *, char *, char *);
static int TIfFileOlderThan(CTX *, char *, char *);
static int TIfFileEqualTime(CTX *, char *, char *);

/**
 * Handle the If token.
 *
 * @param ctx Context.
 * @param isIf Is this a 'if' or a 'test'.
 */
void
TIfDo(CTX *ctx, int isIf)
{ 
	char *token3=NULL, *token1, *token2;
	int ret=0;

	if (GetToken(ctx)<TString) {
		XError(ctx, "Unexpected EOF");
	}

	token1=UStrDup(ctx, SBToString(ctx->tb));

	if (GetToken(ctx)<TString) {
		XError(ctx, "Unexpected EOF");
	}

	token2=UStrDup(ctx, SBToString(ctx->tb));

	if (strcmp(token1, "-d")==0) {
		ret=TIfDir(ctx, Expand(ctx, token2));
	} else if (strcmp(token1, "-e")==0) {
		ret=TIfExists(ctx, Expand(ctx, token2));
	} else if (strcmp(token1, "-f")==0) {
		ret=TIfFile(ctx, Expand(ctx, token2));
	} else if (strcmp(token1, "-s")==0) {
		ret=TIfNonZeroFile(ctx, Expand(ctx, token2));
	} else if (strcmp(token1, "-z")==0) {
		ret=strlen(Expand(ctx, token2));
	} else if (strcmp(token1, "-n")==0) {
		ret=!strlen(Expand(ctx, token2));
	} else {
		if (GetToken(ctx)<TString) {
			XError(ctx, "Unexpected EOF");
		}

		token3=UStrDup(ctx, Expand(ctx, SBToString(ctx->tb)));

		if (strcmp(token2, "-nt")==0) {
			ret=TIfFileNewerThan(ctx, Expand(ctx, token1), token3);
		} else if (strcmp(token2, "-ot")==0) {
			ret=TIfFileOlderThan(ctx, Expand(ctx, token1), token3);
		} else if (strcmp(token2, "-et")==0) {
			ret=TIfFileEqualTime(ctx, Expand(ctx, token1), token3);
		} else if (strcmp(token2, "==")==0) {
			ret=strcmp(Expand(ctx, token1), token3);
		} else if (strcmp(token2, "!=")==0) {
			ret=!strcmp(Expand(ctx, token1), token3);
		} else {
			XError(ctx, "Unknown argument '%s' to if command", token2);
		}
	}

	if (token3) {
		free(token3);
	}

	free(token2);
	free(token1);

	if (isIf) {
		if (ret) {
			FindOptElse(ctx, "if");
		}
	} else {
		ctx->retCode=ret;
	}
}

/**
 * Handle the IfDef token.
 *
 * @param ctx Context.
 */
void
TIfDefDo(CTX *ctx)
{ 
	char *token;
	VARDesc vd;

	if (GetToken(ctx)<TString) {
		XError(ctx, "Unexpected EOF");
	}

	token=Expand(ctx, SBToString(ctx->tb));
	if (!VarGet(ctx->var, token, &vd)) {
		FindOptElse(ctx, "ifdef");
	}
}

/**
 * Handle the IfNDef token.
 *
 * @param ctx Context.
 */
void
TIfNDefDo(CTX *ctx)
{ 
	char *token;
	VARDesc vd;

	if (GetToken(ctx)<TString) {
		XError(ctx, "Unexpected EOF");
	}

	token=Expand(ctx, SBToString(ctx->tb));
	if (VarGet(ctx->var, token, &vd)) {
		FindOptElse(ctx, "ifndef");
	}
}

/**
 * Find optional else block (or endif if no else).
 *
 * @param ctx Context.
 * @param token Token.
 */
static void
FindOptElse(CTX *ctx, char *token)
{
	int depth=1, start=ctx->lineNo;

	while (depth>0) {
		switch (GetToken(ctx)) {
		case TEOF:
			XError(ctx, "Sudden EOF while looking for matching endif to %s starting at %i", token, start);

		case TIf:
		case TIfDef:
		case TIfNDef:
			depth++;
			break;

		case TElse:
			if (depth==1) {
				depth=0;
			}

			break;

		case TEndIf:
			depth--;
			break;
		}
	}
}

/**
 * Stat two files and return the last modified time of both.
 *
 * @param ctx Context.
 * @param lhs File 1.
 * @param rhs File 2.
 * @param lhsDst Where to store modified time for file 1.
 * @param rhsDst Where to store modified time for file 2.
 */
static void
Stat2Files(CTX *ctx, char *lhs, char *rhs, time_t *lhsDst, time_t *rhsDst)
{
	struct stat sBuf;
	char *s;

	if (stat(lhs, &sBuf)==0) {
		*lhsDst=sBuf.st_mtime;
		if (stat(rhs, &sBuf)==0) {
			*rhsDst=sBuf.st_mtime;
			return;
		} else {
			s=rhs;
		}
	} else {
		s=lhs;
	}

	XError(ctx, "Failed to stat %s", s);
}

/**
 * Does entry point to a directory.
 *
 * @param ctx Context.
 * @param src Entry.
 * @return 0 if entry is a directory, !0 otherwise.
 */
static int
TIfDir(CTX *ctx, char *src)
{
	struct stat sBuf;

	if (stat(src, &sBuf)==0) {
		if (S_ISDIR(sBuf.st_mode)) {
			return 0;
		}
	}
	
	return 1;
}

/**
 * Does entry exist in the file system.
 *
 * @param ctx Context.
 * @param src Entry.
 * @return 0 if entry exists, !0 otherwise.
 */
static int
TIfExists(CTX *ctx, char *src)
{
	struct stat sBuf;

	if (stat(src, &sBuf)==0) {
		return 0;
	}
	
	return 1;
}

/**
 * Does entry point to a file.
 *
 * @param ctx Context.
 * @param src Entry.
 * @return 0 if entry is a file, !0 otherwise.
 */
static int
TIfFile(CTX *ctx, char *src)
{
	struct stat sBuf;

	if (stat(src, &sBuf)==0) {
		if (S_ISREG(sBuf.st_mode)) {
			return 0;
		}
	}
	
	return 1;
}

/**
 * Does entry point to a file with content.
 *
 * @param ctx Context.
 * @param src Entry.
 * @return 0 if entry is a file with content, !0 otherwise.
 */
static int
TIfNonZeroFile(CTX *ctx, char *src)
{
	struct stat sBuf;

	if (stat(src, &sBuf)==0) {
		if (S_ISREG(sBuf.st_mode)) {
			if (sBuf.st_size>0) {
				return 0;
			}
		}
	}
	
	return 1;
}

/**
 * Is file(lhs) newer than file(rhs).
 *
 * @param ctx Context.
 * @param src Entry.
 * @return 0 yes, !0 no.
 */
static int
TIfFileNewerThan(CTX *ctx, char *lhs, char *rhs)
{
	time_t l, r;

	Stat2Files(ctx, lhs, rhs, &l, &r);
	if (l>r) {
		return 0;
	}

	return 1;
}

/**
 * Is file(lhs) older than file(rhs).
 *
 * @param ctx Context.
 * @param src Entry.
 * @return 0 yes, !0 no.
 */
static int
TIfFileOlderThan(CTX *ctx, char *lhs, char *rhs)
{
	time_t l, r;

	Stat2Files(ctx, lhs, rhs, &l, &r);
	if (l<r) {
		return 0;
	}

	return 1;
}

/**
 * Is file(lhs) of same time as file(rhs).
 *
 * @param ctx Context.
 * @param src Entry.
 * @return 0 yes, !0 no.
 */
static int
TIfFileEqualTime(CTX *ctx, char *lhs, char *rhs)
{
	time_t l, r;

	Stat2Files(ctx, lhs, rhs, &l, &r);
	if (l==r) {
		return 0;
	}

	return 1;
}

/**
 * Handle Else token.
 *
 * @param ctx Context.
 */
void
TElseDo(CTX *ctx)
{
	int t=1, c=ctx->lineNo;

	for (EVER) {
		switch (GetToken(ctx)) {
		case TEOF:
			XError(ctx, "Sudden EOF while looking for matching endif to else starting at %i", c);

		case TIf:
			t++;
			break;

		case TEndIf:
			t--;
			break;
		}

		if (!t) {
			break;
		}
	}
}
