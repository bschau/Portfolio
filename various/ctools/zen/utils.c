#include "include.h"

/**
 * Set permissions on file.
 *
 * @param path Path to file.
 * @param mode mode.
 */
void set_file_permissions(char *path, int mode)
{
	if (chmod(path, mode)) {
		fprintf(stderr, "Warning: cannot set permissions on %s to %#o\n", path, mode);
	}
}

/**
 * Read a file into memory. Make sure that the file content is nul terminated.
 *
 * @param file File to read.
 * @return string with file content.
 */
char *read_file(char *file)
{
	struct stat buf;
	FILE *fh;
	char *mem;

	if (stat(file, &buf) != 0) {
		perror(file);
		exit(1);
	}

	if ((mem = (char *) malloc(buf.st_size + 1)) == NULL) {
		fprintf(stderr, "Cannot allocate %li bytes\n", (long int) buf.st_size + 1);
		exit(1);
	}

	if ((fh = fopen(file, "rb")) == NULL) {
		perror(file);
		exit(1);
	}

	if (fread(mem, 1, buf.st_size, fh) != buf.st_size) {
		fprintf(stderr, "Cannot read from %s (%s)\n", file, strerror(errno));
		exit(1);
	}

	fclose(fh);

	mem[buf.st_size] = 0;
	return mem;
}
