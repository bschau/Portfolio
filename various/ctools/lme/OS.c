/**
 * OS specific codes.
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Include.h"

/**
 * Return user id from environment.
 *
 * @return user id.
 */
char *
OSUserId(void)
{
#ifdef Linux
	return getlogin();
#else
	return getenv("USERNAME");
#endif
}
