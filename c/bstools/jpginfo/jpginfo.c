#include <sys/types.h>
#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <jpeglib.h>
#define MAIN
#include "common.h"

/* forward decls */
static void usage(int ec);
static int jpginfo(char *file, int *w, int *h);
static void my_error_exit(j_common_ptr cinfo);

/* globals */
static jmp_buf j_buf;

/**
 * Write usage screen and exit.
 *
 * @param ec Exit code.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "jpginfo %s\n", bstools_version);
	fprintf(f, "Usage: jpginfo [OPTIONS] jpg1 jpg2 ... jpgX\n");
	fprintf(f, "\n[OPTIONS]\n\n");
	fprintf(f, " -h       Help (this screen)\n");
	fprintf(f, " -n       Do not keep track of maximum and minimum sizes\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int min_w = INT_MAX, min_h = INT_MAX, max_w = -1, max_h = -1, opt;
	int track = 1, w, h;

	while ((opt = getopt(argc, argv, "hn")) != -1) {
		switch (opt) {
		case 'h':
			usage(0);

		case 'n':
			track = 0;
			break;

		default:
			fprintf(stderr, "Unknown option '%c'\n", opt);
			exit(1);
		}
	}

	if ((argc - optind) == 0) {
		usage(1);
	}

	for (; optind < argc; optind++) {
		printf("%s: ", argv[optind]);
		if (jpginfo(argv[optind], &w, &h)) {
			printf("%d x %d\n", w, h);

			if (track) {
				if (w < min_w) {
					min_w = w;
				} else if (w > max_w) {
					max_w = w;
				}

				if (h < min_h) {
					min_h = h;
				} else if (h > max_h) {
					max_h = h;
				}
			}
		}
	}

	if (track) {
		if ((min_w != INT_MAX) || (min_h != INT_MAX) ||
		    (max_w != -1) || (max_h != -1)) {
			printf("minW: %d   maxW: %d   minH: %d   maxH: %d\n", min_w, max_w, min_h, max_h);
		}
	}

	exit(0);
}

/**
 * Read jpeg file and fetch header info.
 *
 * @param file File to query.
 * @param w Where to store width.
 * @param h Where to store height.
 * @return 0 if error, !0 otherwise.
 */
static int jpginfo(char *file, int *w, int *h) {
	FILE *f = fopen(file, "rb");
	int ec = 1;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	if (f == NULL) {
		perror("fopen");
		return 0;
	}

	cinfo.err = jpeg_std_error(&jerr);
	cinfo.err->error_exit = my_error_exit;

	if (setjmp(j_buf)) {
		jpeg_destroy_decompress(&cinfo);
		fclose(f);
		return 0;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, f);

	ec = 1;
	if (jpeg_read_header(&cinfo, TRUE) == JPEG_HEADER_OK) {
		*w = cinfo.image_width;
		*h = cinfo.image_height;
	} else {
		perror("jpeg_read_header");
		ec = 0;
	}

	jpeg_destroy_decompress(&cinfo);
	fclose(f);

	return ec;
}

static void my_error_exit(j_common_ptr cinfo) 
{
	printf("read error\n");
	longjmp(j_buf, 1);
}
