/**
 * Lexer. A simple lexer hardcoded for LMake.
 */
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "Include.h"

/**
 * Get next character from input stream.
 *
 * @param ctx Context.
 * @return character (as int) or EOF.
 */
int
GetChar(CTX *ctx)
{
	int c;

	if (ctx->ungetc==-1) {
		int zapComment=0;

		do {
			if (ctx->fp) {
				c=fgetc(ctx->fp);
			} else {
				if (*ctx->src) {
					c=*ctx->src++;
				} else {
					c=EOF;
				}
			}

			if (zapComment) {
				if (c=='\n') {
					zapComment=0;
				}
			} else if (ctx->inQuote) {
				if (c==ctx->inQuote) {
					ctx->inQuote=0;
				}
			} else if (c=='#') {
				zapComment=1;
			} else if (c=='\'' || c=='"') {
				ctx->inQuote=1;
			}
		} while (zapComment);
	} else {
		if (!ctx->ungetc) {
			c=EOF;
		} else {
			c=ctx->ungetc;
		}
	}

	if (c==EOF) {
		ctx->ungetc=0;
	} else {
		ctx->ungetc=-1;
	}

	return c;
}

/**
 * Skip whitespaces.
 *
 * @param ctx Context.
 */
void
SkipWhite(CTX *ctx)
{
	int c;

	for (EVER) {
		if ((c=GetChar(ctx))==EOF) {
			break;
		}

		if (c>' ' || c=='\n') {
			break;
		}
	}

	ctx->ungetc=c;
}

/**
 * Get to end of line (discarding comments).
 *
 * @param ctx Context.
 * @return Length of data in ctx->tb.
 */
int
GetEOL(CTX *ctx)
{
	int c;
	char *src;

	SBReset(ctx->tb);

	for (EVER) {
		if ((c=GetChar(ctx))==EOF) {
			break;
		}
		
		if (c=='\n') {
			ctx->ungetc='\n';
			break;
		}

		SBAppendChar(ctx->tb, c);
	}

	src=UStrDup(ctx, UTrim(SBToString(ctx->tb)));
	SBReset(ctx->tb);
	SBAppend(ctx->tb, src, strlen(src));
	free(src);
	return SBLength(ctx->tb);
}

/**
 * Go to next line.
 *
 * @param ctx Context.
 */
void
NextLine(CTX *ctx)
{
	int c;

	for (EVER) {
		if ((c=GetChar(ctx))==EOF) {
			return;
		}

		if (c=='\n') {
			break;
		}
	}

	ctx->lineNo++;
}

/**
 * Get next token from input stream.
 *
 * @param ctx Context.
 * @return Token ID. ctx->tb will be set to the ascii representation of the
 * token.
 */
int
GetToken(CTX *ctx)
{
	char *t;
	int c;

	SBReset(ctx->tb);

	SkipWhite(ctx);

	if ((c=GetChar(ctx))==EOF) {
		return TEOF;
	}

	if (c=='"' || c=='\'') {
		int prevChr=c, term=c;

		SBAppendChar(ctx->tb, c);
		for (EVER) {
			if ((c=GetChar(ctx))==EOF) {
				XError(ctx, "Unterminated string constant");
			}

			SBAppendChar(ctx->tb, c);

			if ((c==term) && (prevChr!='\\')) {
				break;
			}

			prevChr=c;
		}

		return TString;
	}

	if (c=='\n') {
		ctx->lineNo++;
		return TEOL;
	}

	SBAppendChar(ctx->tb, c);
	for (EVER) {
		if ((c=GetChar(ctx))==EOF) {
			break;
		}

		if (isspace((int)c)) {
			ctx->ungetc=c;
			break;
		}

		SBAppendChar(ctx->tb, c);
	}

	t=SBToString(ctx->tb);
	c=SBLength(ctx->tb);

	if (t[c-1]==':') {
		return TProcedure;
	}

	if (strcmp(t, "==")==0) {
		return TEqual;
	}

	if (*t=='=') {
		return TAssignment;
	}

	if (*t=='<') {
		return TListStart;
	}

	if (*t=='>') {
		return TListEnd;
	}

	if (*t=='$' && *(t+1)=='(') {
		return TVariable;
	}

	if (*t=='$') {
		return TPositional;
	}

	if (strcmp(t, "if")==0) {
		return TIf;
	}

	if (strcmp(t, "ifdef")==0) {
		return TIfDef;
	}

	if (strcmp(t, "ifndef")==0) {
		return TIfNDef;
	}

	if (strcmp(t, "else")==0) {
		return TElse;
	}

	if (strcmp(t, "endif")==0) {
		return TEndIf;
	}

	if (strcmp(t, "include")==0) {
		return TInclude;
	}

	if (strcmp(t, "&&")==0) {
		return TAnd;
	}

	if (strcmp(t, "||")==0) {
		return TOr;
	}

	if (strcmp(t, "in")==0) {
		return TIn;
	}

	if (strcmp(t, "msg")==0) {
		return TMsg;
	}

	if (strcmp(t, "test")==0) {
		return TTest;
	}

	return TString;
}

/**
 * Peek next token. This only works for ptr streams.
 *
 * @param ctx Context.
 * @return Token ID. ctx->tb will be set to the ascii representation of the
 * token.
 */
int
Peek(CTX *ctx)
{
	int lineNo=ctx->lineNo, ungetc=ctx->ungetc, id;
	char *src=ctx->src;

	if (!src) {
		XError(ctx, "Peek error");
	}

	id=GetToken(ctx);

	ctx->src=src;
	ctx->ungetc=ungetc;
	ctx->lineNo=lineNo;

	return id;
}
