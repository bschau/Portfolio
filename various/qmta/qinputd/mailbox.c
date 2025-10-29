#include "private.h"

/*******************************************************************************
*
*	validdomain
*
*	Check if given domain is syntactically correct.
*
*	Input:
*		d - domain.
*	Output:
*		domain ok: 0.
*		domain not ok: !0.
*
*	BNF for domain:
*
*	<domain>      ::= <element>|<element> "." <domain>
*	<element>     ::= <name> | "#" <number> | "[" <dotnum> "]"
*	<name>        ::= <a> <ldh-str> <let-dig>
*	<ldh-str>     ::= <let-dig-hyp> | <let-dig-hyp> <ldh-str>
*	<let-dig>     ::= <a> | <d>
*	<let-dig-hyp> ::= <a> | <d> | "-"
*	<number>      ::= <d> | <d> <number>
*	<dotnum>      ::= <snum> "." <snum> "." <snum> "." <snum>
*	<a>           ::= (A-Za-z)
*	<d>           ::= (0-9)
*	<snum>        ::= (0-9){1,3}
*
*	This all sums up to:
*
*		(A-Za-z0-9.#[]-)
*
*	:)
*/
int
validdomain(char *d)
{
	if (!d)
		return 1;

	if (!*d)
		return 1;

	for (; *d; d++) {
		if (isalnum((int)*d))	/* A-Za-z0-9 */
			continue;
		switch (*d) {
			case '.':
			case '#':
			case '[':
			case ']':
			case '-':
				continue;
			default:
				return 1;
		}
	}

	return 0;
}

/*******************************************************************************
*
*	validmailbox
*
*	Is mailbox string valid.
*
*	Input:
*		m - mailbox.
*	Output:
*		mailbox ok: 0.
*		mailbox error: !0.
*/
int
validmailbox(char *m)
{
	int cnt=0;

	for (; *m && *m!='@'; m++)
		cnt++;

	if (!cnt)
		return 1;

	if (*m!='@')
		return 1;

	m++;
	return validdomain(m);
}
