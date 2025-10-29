/**
 * Macro expander.
 */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "Include.h"

/* protos */
static void AddSuffix(CTX *, char *, char *);

/* globals */
extern char dirSepChar;

/**
 * Possible expand the src input with tokens found within the input.
 *
 * @param ctx Context.
 * @param src Source string.
 * @return Expanded string.
 */
char *
Expand(CTX *ctx, char *src)
{
	char buffer[32];
	char *orig, *token;
	VARDesc vd;

	orig=UStrDup(ctx, src);
	src=UTrim(orig);
	SBReset(ctx->exp);
	while (*src) {
		if (*src=='\\') {
			SBAppendChar(ctx->exp, *(src+1));
			src+=2;
		} else if (*src=='$') {
			src++;
			if (*src=='(') {
				for (token=++src; *src && *src!=')'; src++);

				if (!*src) {
					XError(ctx, "Invalid variable reference $('%s'", token);
				}

				*src++=0;
				VarGet(ctx->var, token, &vd);
				if (!*src || isspace((int)*src) || *src=='\\' || *src=='$') {
					if (vd.dLen) {
						SBAppend(ctx->exp, vd.data, vd.dLen-1);
					}
				} else {
					if (vd.dLen) {
						AddSuffix(ctx, vd.data, src);
					}

					for (; *src && !isspace((int)*src); src++);
				}
			} else if (*src=='-') {
				if (VarGet(ctx->var, "-", &vd)) {
					SBAppend(ctx->exp, vd.data, vd.dLen-1);
				}

				src++;
			} else if (*src=='<') {
				VarGet(ctx->var, "<", &vd);
				src++;
				if (!*src || isspace((int)*src) || *src=='\\' || *src=='$') {
					if (vd.dLen) {
						SBAppend(ctx->exp, vd.data, vd.dLen-1);
					}
				} else {
					if (vd.dLen) {
						AddSuffix(ctx, vd.data, src);
					}

					for (; *src && !isspace((int)*src); src++);
				}
			} else if (*src=='?') {
				snprintf(buffer, sizeof(buffer)-1, "%i", ctx->retCode);
				SBAppend(ctx->exp, buffer, strlen(buffer));
				src++;
			} else if (isdigit((int)*src)) {
				char *e;
				int p;

				p=strtol(src, &e, 10);
				src=e;

				if (p==0) {
					if (!*src || isspace((int)*src) || *src=='\\') {
						for (; p<ctx->argc; p++) {
							SBAppend(ctx->exp, ctx->argv[p], strlen(ctx->argv[p]));
							SBAppendChar(ctx->exp, ' ');
						}
					} else {
						for (; p<ctx->argc; p++) {
							AddSuffix(ctx, ctx->argv[p], src);
							SBAppendChar(ctx->exp, ' ');
						}

						for (; *src && !isspace((int)*src); src++);
					}
				} else {
					if (p>ctx->argc) {
						XError(ctx, "No argument at index %i", p);
					}

					p--;
					if (!*src || isspace((int)*src) || *src=='\\') {
						SBAppend(ctx->exp, ctx->argv[p], strlen(ctx->argv[p]));
					} else {
						AddSuffix(ctx, ctx->argv[p], src);
						for (; *src && !isspace((int)*src); src++);
					}
				}
			} else if (*src=='/' || *src=='\\') {
				SBAppendChar(ctx->exp, dirSepChar);
				src++;
			} else if (*src=='[') {
				StringBuilder sbOld, sbNew;
				char *p, *prefix;
				int prefixLen;

				token=++src;
				for (; *src && *src!=']'; src++);
				if (!*src) {
					XError(ctx, "Unterminated expand list");
				}

				*src++=0;

				sbOld=ctx->exp;
				sbNew=SBNew(512);
				ctx->exp=sbNew;
				p=Expand(ctx, token);
				ctx->exp=sbOld;

				for (; *p && isspace((int)*p); p++);
				if (*p) {
					prefix=p;
					for (; *p && !isspace((int)*p); p++);
					prefixLen=p-prefix;

					for (; *p && isspace((int)*p); p++);
					if (*p) {
						StringBuilder sb=SBNew(512);

						while (*p) {
							SBAppend(sb, prefix, prefixLen);
							token=p;
							for (; *p && !isspace((int)*p) && (*p!=']'); p++);
							SBAppend(sb, token, p-token);
							for (; *p && isspace((int)*p); p++) {
								SBAppendChar(sb, *p);
							}
						}

						SBAppend(ctx->exp, SBToString(sb), SBLength(sb));
						SBFree(sb);
					}
				}
				SBFree(sbNew);
			}
		} else {
			SBAppendChar(ctx->exp, *src);
			src++;
		}
	}

	free(orig);
	return SBToString(ctx->exp);
}

/**
 * Add the suffix to all entries in this variable.
 *
 * @param ctx Context.
 * @param rpl Replacement.
 * @param src Source string.
 */
static void
AddSuffix(CTX *ctx, char *rpl, char *src)
{
	int term=0, eLen=0;
	char *ext, *t, *orig;
	char c;

	for (ext=src; *src && !isspace(*src); src++) {
		eLen++;
	}

	c=*src;
	*src=0;
	if (!eLen) {
		*src=c;
		return;
	}

	orig=UStrDup(ctx, rpl);
	rpl=UTrim(orig);
	while (*rpl) {
		if (*rpl=='"' || *rpl=='\'') {
			term=*rpl++;
			SBAppendChar(ctx->exp, term);
			for (t=rpl; *rpl && *rpl!=term; rpl++);

			SBAppend(ctx->exp, t, rpl-t);

			SBAppend(ctx->exp, ext, eLen);
			SBAppendChar(ctx->exp, *rpl);
		} else {
			for (; *rpl && isspace((int)*rpl); rpl++) {
				SBAppendChar(ctx->exp, *rpl);
			}

			for (t=rpl; *rpl && !isspace((int)*rpl); rpl++);

			SBAppend(ctx->exp, t, rpl-t);
			SBAppend(ctx->exp, ext, eLen);
		}
	}
	free(orig);

	*src=c;
}
