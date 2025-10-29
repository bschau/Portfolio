/**
 * Run a given procedure.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Include.h"

/* protos */
static int RunProcedure(CTX *);
static int RunList(CTX *);
static int RunIn(CTX *);
static void HandleString(CTX *, char **);

/**
 * Run procedure.
 *
 * @param var Variables.
 * @param cwd Current working directory.
 * @param proc Procedure to run.
 * @param verbose Verbose.
 * @param argc No. of arguments (may be 0).
 * @param argv Arguments (or NULL).
 * @return retCode.
 */
int
LMake(Var var, char *cwd, char *proc, int verbose, int argc, char **argv)
{
	int retCode=0;
	VARDesc vd;
	CTX *ctx;

	if (!VarGet(var, proc, &vd)) {
		XError(NULL, "No such procedure: '%s'", proc);
	}

	ctx=CTXNew(var, cwd, NULL);
	ctx->lmakeFile=UStrDup(ctx, vd.file);
	ctx->lineNo=vd.lineNo;
	ctx->procedure=proc;
	ctx->verbose=verbose;
	ctx->argc=argc;
	ctx->argv=argv;
	CTXOpen(ctx, vd.data);

	VarSetSystem(var, "PROCEDURE", proc, strlen(proc)+1, 0, NULL);
	XOut(ctx, XOInfo, "Entering %s:%i:%s", ctx->lmakeFile, ctx->lineNo, proc);

	retCode=RunProcedure(ctx);

	XOut(ctx, XOInfo, "Leaving %s:%i:%s", ctx->lmakeFile, ctx->lineNo, proc);

	CTXClose(ctx);
	CTXFree(ctx);
	return retCode;
}

/**
 * Run the given procedure.
 *
 * @param ctx Context.
 * @return retCode.
 */
static int
RunProcedure(CTX *ctx)
{
	char *save=NULL, *tmp;
	int id;

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

			HandleString(ctx, &save);
			break;

		case TProcedure:
			XError(ctx, "Procedures cannot be defined within procedures");

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

			ctx->retCode=0;
			free(save);
			save=NULL;
			break;

		case TListStart:
			ctx->retCode=RunList(ctx);
			break;

		case TListEnd:
			XError(ctx, "End of non-existing list");

		case TVariable:
			if (save) {
				XError(ctx, "Stray garbage '%s'", save);
			}

			tmp=Expand(ctx, SBToString(ctx->tb));
			SBReset(ctx->tb);
			SBAppend(ctx->tb, tmp, strlen(tmp));

			HandleString(ctx, &save);
			break;

		case TIf:
			TIfDo(ctx, IFIsIf);
			ctx->retCode=0;
			break;

		case TIfDef:
			TIfDefDo(ctx);
			ctx->retCode=0;
			break;

		case TIfNDef:
			TIfNDefDo(ctx);
			ctx->retCode=0;
			break;

		case TElse:
			TElseDo(ctx);
			ctx->retCode=0;
			break;

		case TPositional:
			XError(ctx, "Positional accessed out of context");

		case TEndIf:
			break;

		case TEqual:
			XError(ctx, "== can only be used in if-constructs");

		case TInclude:
			XError(ctx, "Cannot include files within procedure");

		case TAnd:
			if (ctx->retCode) {
				NextLine(ctx);
			}

			ctx->retCode=0;
			break;

		case TOr:
			if (!ctx->retCode) {
				NextLine(ctx);
			}

			ctx->retCode=0;
			break;

		case TIn:
			ctx->retCode=RunIn(ctx);
			break;

		case TMsg:
			if (save) {
				XError(ctx, "Stray garbage '%s'", save);
			}

			if ((id=GetEOL(ctx))>0) {
				printf("%s\n", Expand(ctx, SBToString(ctx->tb)));
			}
			break;

		case TTest:
			TIfDo(ctx, IFIsTest);
			break;

		default:
			XError(ctx, "Unknown token '%s', id '%i' returned from GetToken", SBToString(ctx->tb), id);
		}
	}

	return 0;
}

/**
 * Run the given list.
 *
 * @param ctx Context.
 * @return retCode.
 */
static int
RunList(CTX *ctx)
{
	char **argv=NULL;
	int id, argc, term;
	char *list, *proc, *l, *s;

	SBReset(ctx->sb);
	for (EVER) {
		if (((id=Peek(ctx))==TEOF) || (id==TEOL)) {
			XError(ctx, "Unexpected end of list");
		}

		GetToken(ctx);
		if (id==TListEnd) {
			break;
		}

		SBAppend(ctx->sb, SBToString(ctx->tb), SBLength(ctx->tb));
		SBAppendChar(ctx->sb, ' ');
	}

	list=UStrDup(ctx, UTrim(Expand(ctx, SBToString(ctx->sb))));

	if (((id=GetToken(ctx))==TEOF) || (id==TEOL)) {
		XError(ctx, "Unexpected end of list");
	}

	proc=UStrDup(ctx, SBToString(ctx->tb));

	XOptionPickUp(ctx, NULL, &argc, &argv);

	l=list;
	for (EVER) {
		for (; *l && isspace((int)*l); l++);
		if (!*l) {
			break;
		}

		s=l;
		if (*l=='"' || *l=='\'') {
			term=*l++;
			for (; *l && *l!=term; l++);
			if (!*l) {
				XError(ctx, "Unexpected end of quoted entry '%s'", s);
			}

			l++;
		} else {
			for (; *l && !isspace((int)*l); l++);
		}
		
		SBReset(ctx->sb);
		SBAppend(ctx->sb, s, l-s);
		s=Expand(ctx, SBToString(ctx->sb));
		VarSetSystem(ctx->var, "<", s, strlen(s)+1, ctx->lineNo, ctx->lmakeFile);

		if ((ctx->retCode=LMake(ctx->var, ctx->cwd, proc, ctx->verbose, argc, argv))) {
			break;
		}
	}

	XOptionFree(argv);
	free(proc);
	free(list);

	return 0;
}

/**
 * Run command or procedure in the given directory.
 *
 * @param ctx Context.
 * @return retCode.
 */
static int
RunIn(CTX *ctx)
{
	char *save=NULL, *cwd, *s;
	int id;

	SBReset(ctx->sb);

	if (((id=Peek(ctx))==TEOF) || (id==TEOL) || (id==TAnd) || (id==TOr)) {
		XError(ctx, "Usage: in directory [procedure | command]");
	}

	cwd=XGetCwd(ctx);
	VarSetSystem(ctx->var, "PWD", cwd, strlen(cwd)+1, ctx->lineNo, ctx->lmakeFile);

	GetToken(ctx);
	s=Expand(ctx, SBToString(ctx->tb));
	XChDir(ctx, s);
	s=XGetCwd(ctx);
	VarSetSystem(ctx->var, "CWD", s, strlen(s)+1, ctx->lineNo, ctx->lmakeFile);
	free(s);

	if (((id=Peek(ctx))==TEOF) || (id==TEOL) || (id==TAnd) || (id==TOr)) {
		ctx->retCode=0;
	} else {
		GetToken(ctx);
		HandleString(ctx, &save);
		if (save) {
			XError(ctx, "Invalid argument(s) '%&s' given to in", save);
		}
	}

	VarSetSystem(ctx->var, "PWD", s, strlen(s)+1, ctx->lineNo, ctx->lmakeFile);
	XChDir(ctx, cwd);
	VarSetSystem(ctx->var, "CWD", cwd, strlen(cwd)+1, ctx->lineNo, ctx->lmakeFile);

	free(cwd);
	return ctx->retCode;
}

/**
 * Handle a string. This is called by TString, TVariable and TIn.
 *
 * @param ctx Context.
 * @param save Where to save string.
 */
static void
HandleString(CTX *ctx, char **save)
{
	char *tmp=SBToString(ctx->tb);
	int forceSuccess=0, options=1, id;
	char **argv=NULL;
	VARDesc vd;
	int (*func)(CTX *);

	for (EVER) {
		if (*tmp=='-') {
			forceSuccess=1;
			tmp++;
		} else if (*tmp=='%') {
			ctx->forceOutput=1;
			tmp++;
		} else if (*tmp=='*') {
			options=0;
			tmp++;
		} else if (*tmp=='!') {
			ctx->suppressCommand=1;
			tmp++;
		} else {
			break;
		}
	}

	tmp=UStrDup(ctx, tmp);
	if ((id=Peek(ctx))==TAssignment) {
		*save=tmp;
		ctx->forceOutput=0;
		ctx->suppressCommand=0;
		return;
	}

	if (*tmp=='@') {
		ctx->retCode=ExternalRun(ctx, tmp+1);
	} else {
		if (VarGet(ctx->var, tmp, &vd)) {
			argv=NULL;
			if (options) {
				XOptionPickUp(ctx, NULL, &id, &argv);
			}

			ctx->retCode=LMake(ctx->var, ctx->cwd, tmp, ctx->verbose, id, argv);
			XOptionFree(argv);
		} else if ((func=CmdLookup(tmp))) {
			ctx->retCode=func(ctx);
		} else {
			ctx->retCode=ExternalRun(ctx, tmp);
		}
	}

	ctx->suppressCommand=0;

	if (forceSuccess) {
		ctx->retCode=0;
	}

	if (ctx->retCode==0) {
		ctx->forceOutput=0;
	}

	free(tmp);
	return;
}
