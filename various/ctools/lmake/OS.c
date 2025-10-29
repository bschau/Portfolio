/**
 * OS specific codes.
 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "Include.h"

/* globals */
#ifdef Linux
char *P="Linux";
char dirSepChar='/';
static struct {
	char *token;
	char *value;
} syms[]={
	{ "RM", "rm -f" },
	{ "RD", "rm -fr" },
	{ "CC", "gcc" },
	{ "CFLAGS", "-Wall -O2" },
	{ "EXE", "" },
	{ "SO", ".so" },
	{ NULL, NULL }
};
#else
char *P="WIN32";
char dirSepChar='\\';
static struct {
	char *token;
	char *value;
} syms[]= {
	{ "RM", "del /q" },
	{ "RD", "rmdir /s /q" },
	{ "CC", "tcc.exe" },
	{ "CFLAGS", "-Wall" },
	{ "EXE", ".exe" },
	{ "SO", ".dll" },
	{ NULL, NULL }
};
#endif

/**
 * Return user id from environment.
 *
 * @return user id.
 */
char *
OSUserId(void)
{
#ifdef Linux
	return getlogin();
#else
	return getenv("USERNAME");
#endif
}

/**
 * Initialize OS specific variables.
 *
 * @param v Variables.
 */
void
InitVars(Var v)
{
	int i;
		
	for (i=0; syms[i].token; i++) {
		VarSetSystem(v, syms[i].token, syms[i].value, strlen(syms[i].value)+1, 0, NULL);
	}
}
