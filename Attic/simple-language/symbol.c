#include "global.h"

#define STRMAX	(999)
#define SYMMAX	(100)

static char lexemes[STRMAX];
static int lastchar = -1, lastentry = 0;

int lookup(char s[])
{
	for (int p = lastentry; p > 0; p--) {
		if (strcmp(symtable[p].lexptr, s) == 0) {
			return p;
		}
	}

	return 0;
}

int insert(char s[], int tok)
{
	int len = strlen(s);
	if (lastentry + 1 >= SYMMAX) {
		error("symbol table full");
	}

	if (lastchar + len + 1 >= STRMAX) {
		error("lexemes array full");
	}

	lastentry++;
	symtable[lastentry].token = tok;
	symtable[lastentry].lexptr = &lexemes[lastchar + 1];
	lastchar = lastchar + len + 1;
	strcpy(symtable[lastentry].lexptr, s);
	return lastentry;
}
