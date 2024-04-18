#include "global.h"

struct entry keywords[] = {
	{ "div", DIV },
	{ "mod", MOD },
	{ NULL, 0 }
};

void init(void)
{
	for (struct entry *p = keywords; p->token; p++) {
		insert(p->lexptr, p->token);
	}

	lineno = 0;
	tokenval = NONE;
}
