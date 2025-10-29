#include <errno.h>
#include <gd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAIN
#include "common.h"

/* forward decls. */
static void usage(int ec);

/* globals */
extern int errno;

/**
 * Write usage information and exit with ec.
 *
 * @param ec Exit code.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "giftrans %s\n", bstools_version);
	fprintf(f, "Usage: giftrans [OPTIONS] image\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -C         Clear transparency\n");
	fprintf(f, "  -c rrggbb  Set transparency to rrggbb\n");
	fprintf(f, "  -h         Help text (this)\n");
	fprintf(f, "  -i index   Set transparency to color at index\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int query = 1, index = -1, opt, cr, cg, cb;
	FILE *fh;
	gdImagePtr g;
	char *f_name;

	while ((opt = getopt(argc, argv, "Cc:hi:")) != -1) {
		switch (opt) {
		case 'C':
			index = -2;
			query = 0;
			break;

		case 'c':
			if (strlen(optarg) != 6) {
				fprintf(stderr, "Format error (use rrggbb hexadecimal)\n");	
				exit(1);
			}

			if (sscanf(optarg, "%02x%02x%02x", &cr, &cg, &cb) != 3) {
				fprintf(stderr, "Values must be in 0<=value<=255\n");
				exit(1);
			}

			index = -1;
			query = 0;
			break;

		case 'h':
			usage(0);
			break;

		case 'i':
			index = atoi(optarg);
			if ((index < 0) || (index > 255)) {
				fprintf(stderr, "Index out of range (0-255)\n");
				exit(1);
			}

			query = 0;
			break;

		default:
			usage(1);
			break;
		}
	}

	if ((argc - optind) != 1) {
		usage(1);
	}

	f_name = argv[optind];
	if ((fh = fopen(f_name, "r")) == NULL) {
		fprintf(stderr, "Cannot open file %s (%i)\n", argv[optind], errno);
		exit(1);
	}

	if ((g = gdImageCreateFromGif(fh)) == NULL) {
		fprintf(stderr, "Corrupt image or not a GIF file\n");
		exit(1);
	}

	fclose(fh);

	if (query) {
		if ((index = gdImageGetTransparent(g)) == -1) {
			printf("No Transparent color index defined\n");
		} else {
			printf("Transparent color index: %i\n", index);
		}
	} else {
		if (index == -2) {
			index = -1;
		} else if (index == -1) {
			if ((index = gdImageColorExact(g, cr, cg, cb)) == -1) {
				fprintf(stderr, "Cannot find the specified color in the color table\n");
				if ((index = gdImageColorClosest(g, cr, cg, cb)) > -1) {
					fprintf(stderr, "Closest color is at index %i (%02x%02x%02x)\n", index, gdImageRed(g, index), gdImageGreen(g, index), gdImageBlue(g, index));
				}

				exit(1);
			}
		}

		gdImageColorTransparent(g, index);
		
		if ((fh = fopen(f_name, "w")) == NULL) {
			fprintf(stderr, "Cannot reopen file for writing (%i)\n", errno);
			exit(1);
		}

		gdImageGif(g, fh);
		fclose(fh);
	}

	exit(0);
}
