/**
 * Run external program.
 */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Include.h"

/**
 * Run an external Application.
 *
 * @param ctx Context.
 * @param app Application.
 * @param hush Silent? (1=yes, 0=no).
 */
int
ExternalRun(CTX *ctx, char *app)
{
	char *cmd=UStrDup(ctx, app);
	int retCode=0;

	XOut(ctx, XODebug, "> External %s", cmd);

	if (!dryRun) {
		char line[128];
		FILE *fp;
		char *s;
		char c;

		GetEOL(ctx);

		cmd=Expand(ctx, cmd);
		SBReset(ctx->sb);
		for (s=cmd; *s && (!isspace(*s)); s++);
		if (*s) {
			c=*s;
			*s++=0;
			cmd=XRealPath(cmd);
			SBAppend(ctx->sb, cmd, strlen(cmd));
			SBAppendChar(ctx->sb, c);
			SBAppend(ctx->sb, s, strlen(s));
		} else {
			cmd=XRealPath(cmd);
			SBAppend(ctx->sb, cmd, strlen(cmd));
		}

		free(cmd);
		SBAppendChar(ctx->sb, ' ');
		s=Expand(ctx, UTrim(SBToString(ctx->tb)));
		SBAppend(ctx->sb, s, strlen(s));

		if (!ctx->suppressCommand) {
			printf("\t%s\n", SBToString(ctx->sb));
		}

		if ((fp=popen(SBToString(ctx->sb), "r"))==NULL) {
			XError(ctx, "Failed to run '%s'", cmd);
		}

		SBReset(ctx->sb);
		while (fgets(line, sizeof(line), fp)) {
			SBAppend(ctx->sb, line, strlen(line));
		}

		retCode=pclose(fp);

		VarSetSystem(ctx->var, "-", SBToString(ctx->sb), SBLength(ctx->sb)+1, ctx->lineNo, ctx->lmakeFile);
		if (ctx->forceOutput) {
			printf("%s", SBToString(ctx->sb));
			fflush(NULL);
		} else {
			XOut(ctx, XOInfo, "%s", SBToString(ctx->sb));
		}
	} else {
		CmdDryRun(ctx);
	}

	XOut(ctx, XODebug, "< External %s = %i", cmd, retCode);
	return retCode;
}
