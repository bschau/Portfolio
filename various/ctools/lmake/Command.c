/**
 * The build in commands.
 */
#include <ctype.h>
#ifdef Linux
#include <libgen.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Include.h"

/* protos */
static int CmdVarDump(CTX *);
static int CmdDepend(CTX *);
static int CmdLMake(CTX *);
static int CmdWriteFile(CTX *);
static int CmdReadFile(CTX *);
static int CmdExit(CTX *);
static int CmdBasename(CTX *);

/* globals */
static struct {
	char *cmd;
	int (*func)(CTX *);
} CMDS[]= {
	{ "basename", CmdBasename },
	{ "depend", CmdDepend },
	{ "exit", CmdExit },
	{ "lmake", CmdLMake },
	{ "readfile", CmdReadFile },
	{ "vardump", CmdVarDump },
	{ "writefile", CmdWriteFile },
	{ NULL }
};
extern int optIndex;
extern char *optError[];
extern char *optArg;

/**
 * Lookup the given command.
 *
 * @param cmd Command.
 * @return Pointer to function.
 */
int (*
CmdLookup(char *cmd))(CTX *)
{
	int idx;

	for (idx=0; CMDS[idx].cmd; idx++) {
		if (strcmp(CMDS[idx].cmd, cmd)==0) {
			return CMDS[idx].func;
		}
	}

	return NULL;
}

/**
 * Emit dryrun message and go to next line.
 *
 * @param ctx Context.
 */
void
CmdDryRun(CTX *ctx)
{
	XOut(ctx, XOInfo, "No output - dry running ...");
	NextLine(ctx);
}

/*
 * Commands
 */
static int
CmdVarDump(CTX *ctx)
{
	XOut(ctx, XODebug, "> CmdVarDump");
	VarDump(ctx->var);
	XOut(ctx, XODebug, "< CmdVarDump = 0");
	return 0;
}

static int
CmdDepend(CTX *ctx)
{
	int retCode=0;

	XOut(ctx, XODebug, "> CmdDepend");
	if (!dryRun) {
		char *srcFile;
		struct stat sBuf;
		int id;

		if (((id=GetToken(ctx))==TEOF) || (id==TEOL) || (id==TAnd) || (id==TOr)) {
			XError(ctx, "Usage: depend file file1 ... fileX");
		}

		srcFile=UStrDup(ctx, Expand(ctx, SBToString(ctx->tb)));
		if (!stat(Expand(ctx, SBToString(ctx->tb)), &sBuf)) {
			char *f, *p;
			int term;
			time_t master;

			master=sBuf.st_mtime;

			id=Peek(ctx);
			while ((id!=TEOF) && (id!=TEOL) && (id!=TAnd) && (id!=TOr) && (!retCode)) {
				GetToken(ctx);

				p=Expand(ctx, SBToString(ctx->tb));
				while (!retCode) {
					for (; *p && isspace((int)*p); p++);
					if (!*p) {
						break;
					}

					f=p;
					if (*p=='"' || *p=='\'') {
						term=*p++;
						f++;
						for (; *p && *p!=term; p++);
					} else {
						for (; *p && !isspace((int)*p); p++);
					}

					if (*p) {
						*p++=0;
					}

					if (stat(f, &sBuf)) {
						XError(ctx, "Failed to stat '%s'", f);
					}


					if (master<sBuf.st_mtime) {
						retCode=1;
						break;
					}
					XOut(ctx, XODebug, "master '%s' is newer than '%s'", srcFile, f);
				}

				id=Peek(ctx);
			}
		} else {
			XOut(ctx, XODebug, "no such file: '%s'", srcFile);
			retCode=1;
		}

		free(srcFile);

		while (((id=Peek(ctx))!=TEOF) && (id!=TEOL) && (id!=TAnd) && (id!=TOr)) {
			GetToken(ctx);
		}
	} else {
		CmdDryRun(ctx);
	}

	XOut(ctx, XODebug, "< CmdDepend = %i", retCode);
	return retCode;
}

static int
CmdLMake(CTX *ctx)
{
	XOut(ctx, XODebug, "> CmdLMake");
	if (!dryRun) {
		Var var=VarNew(ctx->var);
		char **argv=NULL;
		int argc;

		XOptionPickUp(ctx, "lmake", &argc, &argv);
		Run(var, argc, argv, ctx->verbose);
		XOptionFree(argv);
		VarFree(var);
	} else {
		CmdDryRun(ctx);
	}

	XOut(ctx, XODebug, "< CmdLMake = 0");
	return 0;
}

static int
CmdWriteFile(CTX *ctx)
{
	XOut(ctx, XODebug, "> CmdWriteFile");
	if (!dryRun) {
		int total=0, len, id;
		FILE *fp;
		char *file, *s;

		if (((id=GetToken(ctx))==TEOF) || (id==TEOL) || (id==TAnd) || (id==TOr)) {
			XError(ctx, "Usage: writefile file i1 ... iX");
		}

		file=UStrDup(ctx, Expand(ctx, SBToString(ctx->tb)));

		if ((fp=fopen(file, "wb"))==NULL) {
			XError(ctx, "Failed to open '%s' for writing", file);
		}

		id=Peek(ctx);
		while ((id!=TEOF) && (id!=TEOL) && (id!=TAnd) && (id!=TOr)) {
			GetToken(ctx);

			s=SBToString(ctx->tb);
			if (*s=='"' || *s=='\'') {
				SBSetLength(ctx->tb, SBLength(ctx->tb)-1);
				s++;
			}

			s=Expand(ctx, s);
			len=strlen(s);
			if (fwrite(s, len, 1, fp)!=1) {
				XError(ctx, "Failed to write to '%s'", file);
			}

			total+=len;
			id=Peek(ctx);
		}

		XOut(ctx, XOInfo, "Wrote %i byte%sto %s", total, (total==1 ? " " : "s "), file);

		fclose(fp);
		free(file);
	} else {
		CmdDryRun(ctx);
	}

	XOut(ctx, XODebug, "< CmdWriteFile = 0");
	return 0;
}

static int
CmdReadFile(CTX *ctx)
{
	XOut(ctx, XODebug, "> CmdReadFile");
	if (!dryRun) {
		int total=0, len, id;
		char line[128];
		FILE *fp;
		char *var, *s;

		if (((id=GetToken(ctx))==TEOF) || (id==TEOL) || (id==TAnd) || (id==TOr)) {
			XError(ctx, "Usage: readfile var file i1 ... iX");
		}

		var=UStrDup(ctx, Expand(ctx, SBToString(ctx->tb)));

		SBReset(ctx->sb);

		id=Peek(ctx);
		while ((id!=TEOF) && (id!=TEOL) && (id!=TAnd) && (id!=TOr)) {
			GetToken(ctx);

			s=SBToString(ctx->tb);
			if (*s=='"' || *s=='\'') {
				SBSetLength(ctx->tb, SBLength(ctx->tb)-1);
				s++;
			}

			s=Expand(ctx, s);

			if ((fp=fopen(s, "rb"))==NULL) {
				XError(ctx, "Failed to open '%s' for reading", s);
			}

			while (fgets(line, sizeof(line), fp)) {
				len=strlen(line);
				SBAppend(ctx->sb, line, len);
				total+=len;
			}

			fclose(fp);

			id=Peek(ctx);
		}

		XOut(ctx, XOInfo, "Read %i byte%s into %s", total, (total==1 ? " " : "s "), var);

		SBAppendChar(ctx->sb, 0);
		VarSet(ctx->var, var, SBToString(ctx->sb), SBLength(ctx->sb), ctx->lineNo, ctx->lmakeFile);
		free(var);
	} else {
		CmdDryRun(ctx);
	}

	XOut(ctx, XODebug, "< CmdReadFile = 0");
	return 0;
}

static int
CmdExit(CTX *ctx)
{
	XOut(ctx, XODebug, "> CmdExit");
	if (!dryRun) {
		int ret;
		char *s;

		if (((ret=Peek(ctx))==TEOF) || (ret==TEOL) || (ret==TAnd) || (ret==TOr)) {
			ret=0;
		} else {
			GetToken(ctx);
			s=Expand(ctx, SBToString(ctx->tb));
			ret=atoi(s);
		}

		XOut(ctx, XOInfo, "Exiting with code %i", ret);
		exit(ret);
	} else {
		CmdDryRun(ctx);
	}

	XOut(ctx, XODebug, "< CmdExit = 0");
	return 0;
}

static int
CmdBasename(CTX *ctx)
{
	int id, tLen;
	char *var, *s, *p;

	XOut(ctx, XODebug, "> CmdBasename");
	if (((id=GetToken(ctx))==TEOF) || (id==TEOL) || (id==TAnd) || (id==TOr)) {
		XError(ctx, "Usage: basename var path1 ... pathX");
	}

	var=UStrDup(ctx, Expand(ctx, SBToString(ctx->tb)));

	SBReset(ctx->sb);

	id=Peek(ctx);
	while ((id!=TEOF) && (id!=TEOL) && (id!=TAnd) && (id!=TOr)) {
		GetToken(ctx);

		s=SBToString(ctx->tb);
		if (*s=='"' || *s=='\'') {
			SBSetLength(ctx->tb, SBLength(ctx->tb)-1);
			s++;
		}

		s=Expand(ctx, s);
		while ((p=XGetToken(ctx, s, &tLen, &s))!=NULL) {
			if (*s) {
				*s++=0;
			}
			p=basename(p);
			SBAppend(ctx->sb, p, strlen(p));
			SBAppendChar(ctx->sb, ' ');
		}

		id=Peek(ctx);
	}

	SBAppendChar(ctx->sb, 0);
	VarSet(ctx->var, var, SBToString(ctx->sb), SBLength(ctx->sb), ctx->lineNo, ctx->lmakeFile);
	free(var);

	XOut(ctx, XODebug, "< CmdBasename = 0");
	return 0;
}
