/**
 * Options parsing and handling.
 */
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Include.h"

/* globals */
int optIndex;
char optError[64];
char *optArg;

/**
 * Initialize options.
 */
void
OptionInit(void)
{
	optIndex=1;
	optArg=NULL;
}

/**
 * Get next option (pair) from the argument list.
 *
 * @param argc Arg count.
 * @param argv Arg list.
 * @param optStr Option string.
 * @return option.
 */
int
OptionNext(int argc, char * const *argv, const char *optStr)
{
	char *p;

	if ((optIndex>=argc) || (argv[optIndex][0]!='-'))
		return -1;

	if (argv[optIndex][1]=='-') {
		optIndex++;
		return -1;
	}

	if ((p=strchr(optStr, argv[optIndex][1]))) {
		if (p[1]==':') {
			if (argv[optIndex][2])
				optArg=&argv[optIndex][2];
			else {
				optIndex++;
				if (optIndex>=argc) {
					snprintf(optError, sizeof(optError), "no argument for option %c", *p);
					return '?';
				}

				optArg=argv[optIndex];
			}
		}

		optIndex++;
		return (int)*p;
	}

	snprintf(optError, sizeof(optError), "no such option '%c'", argv[optIndex][1]);
	return '?';
}
