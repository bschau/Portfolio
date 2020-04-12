#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#define MAIN
#include "common.h"

/* forward decls */
static void usage(int ec);
static void d2u(char *file);

/**
 * Usage.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "d2u %s\n", bstools_version);
	fprintf(f, "Usage: d2u file1 file2 ... fileN\n");

	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int i;

	if (argc < 2) {
		usage(1);
	}

	for (i = 1; i < argc; i++) {
		d2u(argv[i]);
	}

	exit(0);
}

/**
 * @param file File to convert.
 */
static void d2u(char *file) {
	FILE *fh;
	struct stat sbuf;
	char *mem, *s;
	long len, idx, r;

	if (stat(file, &sbuf)) {
		fprintf(stderr, "Failed to stat %s\n", file);
		exit(1);
	}

	if ((mem = malloc(sbuf.st_size + 1)) == NULL) {
		fprintf(stderr, "OOM\n");
		exit(1);
	}
	mem[sbuf.st_size] = 0;

	if ((fh = fopen(file, "rb")) == NULL) {
		fprintf(stderr, "Failed to open %s for reading\n", file);
		exit(1);
	}

	len = (long) sbuf.st_size;
	idx = 0;
	while (len > 0) {
		r = fread(mem + idx, 1, len, fh);
		if (r > 0) {
			len -= r;
			idx += r;
		}
	}

	fclose(fh);

	if ((fh = fopen(file, "wb+")) == NULL) {
		fprintf(stderr, "Failed to open %s for writing\n", file);
		exit(1);
	}

	len = (long) sbuf.st_size;
	s = mem;
	while (len > 0) {
		if (*s == '\r') {
			if ((*(s + 1) == '\n') || (!*(s + 1))) {
				fputc((int) '\n', fh);
				s++;
				len--;
			} else {
				fputc((int) '\r', fh);
			}
		} else {
			fputc((int) *s, fh);
		}

		s++;
		len--;
	}

	fclose(fh);
	free(mem);
}
