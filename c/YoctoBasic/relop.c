#include "yoctobasic.h"

#define LT (1)
#define LE (LT + 1)
#define GT (LE + 1)
#define GE (GT + 1)
#define NE (GE + 1)
#define EQ (NE + 1)

/**
 * Parse relops pointed to by ip.
 *
 * Relops:
 *
 * 	< | <= | > | >= | <> | =
 *
 * Return relop-type to be used by relop_evaluate.
 */
int relop_pickup(void)
{
	skip_whitespaces();

	if (*ip == '<') {
		if (*(ip + 1) == '=') {
			ip += 2;
			return LE;
		}

		if (*(ip + 1) == '>') {
			ip += 2;
			return NE;
		}

		ip++;
		return LT;
	}

	if (*ip == '>') {
		if (*(ip + 1) == '=') {
			ip += 2;
			return GE;
		}

		ip++;
		return GT;
	}

	if (*ip != '=') {
		error("unknown relop");
	}

	ip++;
	return EQ;
}

/**
 * Evaluate relop:
 *
 *	lhs relop rhs
 *
 * Returns true/false.
 */
int relop_evaluate(int relop, int lhs, int rhs)
{
	switch (relop) {
		case LT:
			return lhs < rhs;
		case LE:
			return lhs <= rhs;
		case GT:
			return lhs > rhs;
		case GE:
			return lhs >= rhs;
		case NE:
			return lhs != rhs;
		case EQ:
			return lhs == rhs;
	}

	error("unknow relop in evaluation");
	exit(1);	/* Keep compiler happy. */
}
