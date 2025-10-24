#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAIN
#include "common.h"

/** forward decls */
static void usage(int ec);
static int get_hexvalue(char d);
static char *get_octet(char *ptr, int *v);
static char *get_glvalue(int *v, char *src, char *orig);
static void get_gloctet(char *ptr, int *v1, int *v2, int *v3);
static char *get_rgbvalue(int *v, char *src, char *orig);
static void get_rgboctet(char *ptr, int *v1, int *v2, int *v3);

/**
 * Write usage information.
 *
 * @param ec Exit code.
 */
static void usage(int ec) {
	FILE *f = stdout;

	if (ec) {
		f = stderr;
	}

	fprintf(f, "colconv %s\n", bstools_version);
	fprintf(f, "Usage: colconv -h | xxxxxx | r,g,b | r-gl,g-gl,b-gl\n");
	fprintf(f, "  -h               This page\n");
	fprintf(f, "  xxxxxx           Hex code\n");
	fprintf(f, "  r,g,b            RGB codes\n");
	fprintf(f, "  r-gl,g-gl,b-gl   OpenGL RGB codes\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int v1 = 0, v2 = 0, v3 = 0;
	char *ptr;

	if (argc != 2) {
		usage(1);
	}

	if (strcmp(argv[1], "-h") == 0) {
		usage(0);
	}

	ptr = argv[1];
	if (strlen(ptr) == 6) {
		ptr = get_octet(ptr, &v1);
		ptr = get_octet(ptr, &v2);
		ptr = get_octet(ptr, &v3);
	} else if (strchr(ptr, (int) '.')) {
		get_gloctet(ptr, &v1, &v2, &v3);
	} else {
		get_rgboctet(ptr, &v1, &v2, &v3);
	}

	printf("Hexcolor:   %X%X%X\r\n", v1, v2, v3);
	printf("RGB:        %i, %i, %i\r\n", v1, v2, v3);
	printf("OpenGL ES:  %.2f, %.2f, %.2f\r\n", (float)v1 / 255.0, (float)v2 / 255.0, (float)v3 / 255.0);
	exit(0);
}

/**
 * Get value of digit.
 * 
 * @param d Digit.
 * @return value.
 */
static int get_hexvalue(char d) {
	if ((d >= '0') && (d <= '9')) {
		return d - '0';
	}

	if ((d >= 'a') && (d <= 'f')) {
		return d - 'a' + 10;
	}

	if ((d >= 'A') && (d <= 'F')) {
		return d - 'A' + 10;
	}

	fprintf(stderr, "Invalid digit: %c\r\n", d);
	exit(1);
}

/**
 * Return the next hex octet to integer.
 * 
 * @param ptr Pointer into string.
 * @param v Where to store value.
 * @return updated pointer.
 */
static char *get_octet(char *ptr, int *v) {
	if (*ptr) {
		*v = get_hexvalue(*ptr++);
		if (*ptr) {
			*v *= 16;
			*v |= get_hexvalue(*ptr++);
		}
	} else {
		*v = 0;
	}

	return ptr;
}

/**
 * Return next gl octet to integer.
 *
 * @param v Where to store value.
 * @param src Source.
 * @param orig Orig string.
 * @return update src.
 */
static char *get_glvalue(int *v, char *src, char *orig) {
	char c;

	if (src == NULL) {
		return NULL;
	}

	if (*src == '0') {
		src++;
		if (*src == '.') {
			src++;
			c = *src++;
			if ((c >= '0') && (c <= '9')) {
				*v = (c - '0') * 10;

				c = *src++;
				if ((c >= '0') && (c <= '9')) {
					*v += (c - '0');
					*v = (*v * 255) / 100;
					return src;
				}
			}
		}
	}

	fprintf(stderr, "Invalid gloctet specifier: %s\n", orig);
	return NULL;
}

/**
 * Return the next gl octet to integer.
 * 
 * @param ptr Pointer into string.
 * @param v1 v1 value.
 * @param v2 v2 value.
 * @param v3 v3 value.
 */
static void get_gloctet(char *ptr, int *v1, int *v2, int *v3) {
	char *p = ptr;

	ptr = get_glvalue(v1, ptr, p);
	if ((ptr) && (*ptr++ == ',')) {
		ptr = get_glvalue(v2, ptr, p);
		if ((ptr) && (*ptr++ == ',')) {
			ptr = get_glvalue(v3, ptr, p);
			return;
		}
	}

	exit(1);
}

/**
 * Return next rgb octet to integer.
 *
 * @param v Where to store value.
 * @param src Source.
 * @param orig Orig string.
 * @return update src.
 */
static char *get_rgbvalue(int *v, char *src, char *orig) {
	char c;
	int i;

	if (src == NULL) {
		return NULL;
	}

	for (i = 0; i < 4; i++) {
		c = *src++;
		if ((c >= '0') && (c <= '9')) {
			*v *= 10;
			*v += (c - '0');
		} else if (c == ',') {
			src--;
		} else if (!c) {
			break;
		}
	}

	if (!*src) {
		src = NULL;
	}

	if ((*v >= 0) && (*v <= 255)) {
		return src;
	}

	fprintf(stderr, "Invalid rgboctet specifier: %s\n", orig);
	return NULL;
}

/**
 * Return the next rgb octet to integer.
 * 
 * @param ptr Pointer into string.
 * @param v Where to store value.
 */
static void get_rgboctet(char *ptr, int *v1, int *v2, int *v3) {
	char *p = ptr;

	ptr = get_rgbvalue(v1, ptr, p);
	if ((ptr) && (*ptr++ == ',')) {
		ptr = get_rgbvalue(v2, ptr, p);
		if ((ptr) && (*ptr++ == ',')) {
			ptr = get_rgbvalue(v3, ptr, p);
			return;
		}
	}

	exit(1);
}
