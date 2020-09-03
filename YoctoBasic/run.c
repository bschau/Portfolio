#include "yoctobasic.h"

/* forwards */
static void parse_label(void);

/**
 * Main loop - parse and divert.
 */
void run(void)
{
	for (;;) {
		skip_whitespaces();
		if (isdigit((int)*ip)) {
			parse_label();
			skip_whitespaces();
		}

		if (*ip == '\n') {
			ip++;
			line_number++;
			continue;
		}

		statement();
	}
}

/**
 * Parse a label. A label is just a set of connected digits at the start
 * of the line.
 */
static void parse_label(void)
{
	while (isdigit((int)*++ip)) {
		ip++;
	}
}

/**
 * Handle statement.
 */
void statement()
{
	if (*ip == '\n') {
		ip++;
		line_number++;
		return;
	}

	if (strncasecmp(ip, "clear", 5) == 0) {
		kw_clear();
	} else if (strncasecmp(ip, "end", 3) == 0) {
		kw_end();
	} else if (strncasecmp(ip, "for", 3) == 0) {
		kw_for();
	} else if (strncasecmp(ip, "gosub", 5) == 0) {
		kw_gosub();
	} else if (strncasecmp(ip, "goto", 4) == 0) {
		kw_goto();
	} else if (strncasecmp(ip, "if", 2) == 0) {
		kw_if();
	} else if (strncasecmp(ip, "input", 5) == 0) {
		kw_input();
	} else if (strncasecmp(ip, "let", 3) == 0) {
		kw_let();
	} else if (strncasecmp(ip, "next", 4) == 0) {
		kw_next();
	} else if (strncasecmp(ip, "print", 5) == 0) {
		kw_print();
	} else if (strncasecmp(ip, "rem", 3) == 0) {
		kw_rem();
	} else if (strncasecmp(ip, "return", 6) == 0) {
		kw_return();
	} else {
		error("unknown statement");
	}
}
