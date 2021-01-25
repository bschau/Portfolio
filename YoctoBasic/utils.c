#include "yoctobasic.h"

/**
 * Skip to end of line. Do not accept non parsed between last allowed
 * character and the eol terminator.
 */
void no_junk_eol(int skip)
{
	char *save = ip;

	ip += skip;

	while (*ip != 0) {
		if (*ip == '\n') {
			line_number++;
			ip++;
			return;
		}

		if (!isspace((int) *ip)) {
			*(save + skip) = 0;
			error("junk after '%s'", save);
		}

		ip++;
	}

	exit(0);
}

/**
 * Skip all whitespace. Do not skip eol.
 */
void skip_whitespaces(void)
{
	while (*ip != 0) {
		if (*ip == ' ' || *ip == '\f' || *ip == '\t' || *ip == '\v' || *ip == '\r') {
			ip++;
		} else {
			return;
		}
	}

	exit(0);
}

/**
 * Parse and pickup a variable.
 */
int pickup_variable(int skip)
{
	ip += skip;
	skip_whitespaces();

	char c = toupper((int) *ip++);
	if (c >= 'A' || c <= 'Z') {
		return c - 'A';
	}

	error("unknown variable: '%c'", c);
}

/**
 * Match the next char to 'c'.
 */
void match(char c)
{
	skip_whitespaces();
	if (*ip != c) {
		error("expected '%c'", c);
	}

	ip++;
}

/**
 * Find eol accepting whatever junk may be seen.
 */
void eol(void)
{
	for (; *ip != 0; ip++) {
		if (*ip != '\n') {
			continue;
		}

		ip++;
		line_number++;
		break;
	}
}

/**
 * Locate a label.
 */
char *locate_label(int label)
{
	char buffer[16];

	snprintf(buffer, sizeof(buffer), "%i", label);

	int label_len = strlen(buffer);
	char *ptr = prom;
	for (; *ptr != 0; ) {
		if (strncasecmp(ptr, buffer, label_len) == 0) {
			if (isspace((int)*(ptr + label_len))) {
				return ptr + label_len;
			}
		}

		for (; *ptr != 0 && *ptr != '\n'; ptr++);
		if (*ptr == '\n') {
			ptr++;
		}
	}

	error("no such label: '%s'", label);
	exit(1);	/* Keep compiler happy. */
}
