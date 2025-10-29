/**
 * Main parser. Parse and initialize procedures, variables et. al.
 */
#include <string.h>
#include <stdlib.h>
#include "Include.h"

/**
 * Parse a LMakefile and any sub LMakefiles.
 *
 * @param ctx Context.
 */
void
LMParse(CTX *ctx)
{
	char *save=NULL, *tmp;
	int id, c, procStart;
	CTX *subCtx;

	for (EVER) {
		if ((id=GetToken(ctx))==TEOF) {
			break;
		}

		if (id==TEOL) {
			continue;
		}

		switch (id) {
		case TString:
			if (save) {
				XError(ctx, "Stray garbage '%s'", save);
			}

			save=UStrDup(ctx, SBToString(ctx->tb));
			break;

		case TProcedure:
			if (save) {
				XError(ctx, "Stray garbage '%s'", save);
			}

			save=UStrDup(ctx, SBToString(ctx->tb));
			c=strlen(save);
			save[c-1]=0;

			SBReset(ctx->sb);
			procStart=ctx->lineNo;

			if ((c=GetEOL(ctx))>0) {
				SBAppend(ctx->sb, SBToString(ctx->tb), c);
				SBAppendChar(ctx->sb, '\n');
			}

			NextLine(ctx);
			for (EVER) {
				if ((c=GetEOL(ctx))==0) {
					break;
				}

				SBAppend(ctx->sb, SBToString(ctx->tb), c);
				SBAppendChar(ctx->sb, '\n');
				NextLine(ctx);
			}

			SBAppendChar(ctx->sb, 0);
			SBAppendChar(ctx->sb, 0);

			VarSet(ctx->var, save, SBToString(ctx->sb), SBLength(ctx->sb)+1, procStart, ctx->lmakeFile);

			free(save);
			save=NULL;
			break;

		case TAssignment:
			if (!save) {
				XError(ctx, "No receiver in assignment");
			}

			if ((id=GetEOL(ctx))>0) {
				tmp=Expand(ctx, SBToString(ctx->tb));
				VarSet(ctx->var, save, tmp, strlen(tmp)+1, 0, NULL);
			} else {
				VarDelete(ctx->var, save);
			}

			free(save);
			save=NULL;
			break;

		case TVariable:
			XError(ctx, "Variable accessed out of context");

		case TListStart:
		case TListEnd:
			XError(ctx, "Lists cannot be used outside procedures");

		case TPositional:
			XError(ctx, "Positional variables cannot be used outside procedures");

		case TIf:
			TIfDo(ctx, IFIsIf);
			break;

		case TIfDef:
			TIfDefDo(ctx);
			break;

		case TIfNDef:
			TIfNDefDo(ctx);
			break;

		case TElse:
			TElseDo(ctx);
			break;

		case TEndIf:
			break;

		case TInclude:
			if (save) {
				XError(ctx, "Stray garbage '%s'", save);
			}

			if (GetToken(ctx)!=TString) {
				XError(ctx, "Invalid or no argument to 'include'");
			}

			subCtx=CTXNew(ctx->var, ctx->cwd, Expand(ctx, SBToString(ctx->tb)));
			CTXOpen(subCtx, NULL);
			LMParse(subCtx);
			CTXClose(subCtx);
			CTXFree(subCtx);
			break;

		case TEqual:
			XError(ctx, "== can only be used in if-structures");

		case TAnd:
			XError(ctx, "&& cannot be used outside procedures");

		case TOr:
			XError(ctx, "|| cannot be used outside procedures");

		case TExit:
			XError(ctx, "exit cannot be used outside procedures");

		case TIn:
			XError(ctx, "in cannot be used outside procedures");

		case TMsg:
			if (save) {
				XError(ctx, "Stray garbage '%s'", save);
			}

			if ((id=GetEOL(ctx))>0) {
				printf("%s\n", Expand(ctx, SBToString(ctx->tb)));
			}
			break;

		case TTest:
			XError(ctx, "test cannot be used outside procedures");

		default:
			XError(ctx, "Unknown token '%s', id '%i' returned from GetToken", SBToString(ctx->tb), id);
		}
	}

	if (save) {
		free(save);
	}
}
