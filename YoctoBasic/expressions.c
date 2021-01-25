#include "yoctobasic.h"

/* forwards */
static int term(void);
static int factor(void);

/**
 * Parse expression pointed to by ip.
 *
 * Expression:
 *
 *	expression ::= (+|-) term ((+|-) term)*
 *	term ::= factor ((*|/) factor)*
 *	factor ::= var | number | (expression)
 *
 * Return value of expression.
 */
int expression(void)
{
	int multiplier = 1;
	int result = 0;

	skip_whitespaces();
	if (*ip == '\n') {
		error("expected expression");
	}

	if (*ip == '+') {
		ip++;
	} else if (*ip == '-') {
		ip++;
		multiplier = -1;
	}

	for (EVER) {
		result += multiplier * term();
		skip_whitespaces();
		if (*ip == '+') {
			multiplier = 1;
			ip++;
		} else if (*ip == '-') {
			multiplier = -1;
			ip++;
		} else {
			break;
		}
	}

	return result;
}

static int term(void)
{
	skip_whitespaces();
	int result = factor();

	for (EVER) {
		skip_whitespaces();
		if (*ip == '\n') {
			break;
		}

		if (*ip == '*') {
			ip++;
			skip_whitespaces();
			int rhs = factor();
			result *= rhs;
		} else if (*ip == '/') {
			ip++;
			skip_whitespaces();
			int rhs = factor();
			result /= rhs;
		} else {
			break;
		}
	}

	return result;
}

static int factor(void)
{
	int result = 0;

	if (*ip == '(') {
		ip++;
		result += expression();
		match(')');
		return result;
	}

	if (isdigit((int)*ip)) {
		while (isdigit((int)*ip)) {
			result *= 10;
			result += *ip - '0';
			ip++;
		}

		return result;
	}

	int var = pickup_variable(0);
	return variables[var];
}
