/**
 * Windows specific utilities.
 */
#include <stdlib.h>
#include <string.h>

/**
 * Return the last component of this path.
 *
 * @param path Path.
 * @return Pointer to last component.
 */
char *
basename(char *path)
{
	char *p=strrchr(path, '\\');

	if (p) {
		return ++p;
	}

	if ((p=strrchr(path, '/'))) {
		return ++p;
	}

	return path;
}
