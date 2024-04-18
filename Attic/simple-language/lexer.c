#include "global.h"

static char lexbuf[BSIZE];

int lexan(void)
{
	int t = 0;

	while (1) {
		t = getchar();
		if (t == ' ' || t == '\t') {
			;
		} else if (t == '\n') {
			lineno++;
		} else if (isdigit(t)) {
			ungetc(t, stdin);
			t = scanf("%d", &tokenval);
			return NUM;
		} else if (isalpha(t)) {
			int b = 0;

			while (isalnum(t)) {
				lexbuf[b++] = t;
				t = getchar();
				if (b >= BSIZE) {
					error("compiler error");
				}
			}

			lexbuf[b] = EOS;
			if (t != EOF) {
				ungetc(t, stdin);
			}

			int p = lookup(lexbuf);
			if (!p) {
				p = insert(lexbuf, ID);
			}

			tokenval = p;
			return symtable[p].token;
		} else if (t == EOF) {
			return DONE;
		} else {
			break;
		}
	}

	tokenval = NONE;
	return t;
}
