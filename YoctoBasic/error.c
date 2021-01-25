#include "yoctobasic.h"

/**
 * Show error message and exit.
 */
void error(char *format, ...)
{
	va_list args;

	fprintf(stderr, "%i: ", line_number);

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	fprintf(stderr, "\n");
	exit(1);
}
