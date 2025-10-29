#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gd.h"
#define MAIN
#include "common.h"

/* macros */
#define CSizeOfSpaceChar 8
#define CNumOfGlyphs 96
#define CGlyphsPrLine 8
#define CGlyphSize 32
#define CBackColor 0x000000
#define CForeColor 0xffffff
#define CFontPath "font.png"
#define CTextPath "font.txt"

/* forward decls */
static void usage(int ec);

/* globals */
static int size_of_space = CSizeOfSpaceChar, start_char = 0x20;
static int num_of_glyphs = CNumOfGlyphs;
static int back_color = CBackColor, fore_color = CForeColor;
static int glyphs_pr_line = CGlyphsPrLine, glyph_size = CGlyphSize;
static char *font_out = CFontPath, *text_out = CTextPath;

/**
 * Write usage screen and exit.
 *
 * @param ec Exit code.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "glttf %s\n", bstools_version);
	fprintf(f, "Usage: glttf [OPTIONS] path-to-font font-size\n");
	fprintf(f, "\n[OPTIONS]\n\n");
	fprintf(f, " -B back  Background color (defaults to %06X)\n", CBackColor);
	fprintf(f, " -C fore  Foreground color (defaults to %06X)\n", CForeColor);
	fprintf(f, " -S size  Size of space character (defaults to %i)\n", CSizeOfSpaceChar);
	fprintf(f, " -c char  Start character (defaults to 0x20 - space)\n");
	fprintf(f, " -f path  Output font path (defaults to %s)\n", CFontPath);
	fprintf(f, " -g size  Glyph size (defaults to %i)\n", CGlyphSize);
	fprintf(f, " -h       Help (this screen)\n");
	fprintf(f, " -l cnt   Glyphs pr. line (defaults to %i)\n", CGlyphsPrLine);
	fprintf(f, " -n cnt   Number of glyphs (defaults to %i)\n", CNumOfGlyphs);
	fprintf(f, " -t path  Output fontsize path (defaults to %s)\n", CTextPath);
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int x = 0, y = 0, idx = 0, baseline = 0;
	int font_size, chr, opt, tmp, all_glyphs, fore;
	gdImagePtr im;
	char *err_str, *path_to_font;
	int b_rect[8];
	FILE *png_out, *f;
	char glyph[2];

	while ((opt = getopt(argc, argv, "B:C:S:c:f:hl:n:t:")) != -1) {
		switch (opt) {
		case 'B':
			sscanf(optarg, "%x", &back_color);
			break;

		case 'C':
			sscanf(optarg, "%x", &fore_color);
			break;

		case 'S':
			size_of_space = atoi(optarg);
			break;

		case 'c':
			start_char = atoi(optarg);
			break;

		case 'f':
			font_out = optarg;
			break;

		case 'g':
			glyph_size = atoi(optarg);
			break;

		case 'h':
			usage(0);

		case 'l':
			glyphs_pr_line = atoi(optarg);
			break;

		case 'n':
			num_of_glyphs = atoi(optarg);
			break;

		case 't':
			text_out = optarg;
			break;

		default:
			fprintf(stderr, "Unknown option '%c'\n", opt);
			exit(1);
		}
	}

	if ((argc - optind) != 2) {
		usage(1);
	}

	path_to_font = argv[optind++];
	font_size = atoi(argv[optind]);

	all_glyphs = ((num_of_glyphs + (glyphs_pr_line - 1)) / glyphs_pr_line) * glyphs_pr_line;

	if ((f = fopen(text_out, "w")) == NULL) {
		perror(text_out);
		exit(1);
	}

	im = gdImageCreate(glyphs_pr_line * glyph_size, (all_glyphs / glyphs_pr_line) * glyph_size);
	gdImageColorAllocate(im, (back_color >> 16) & 0xff, (back_color >> 8) & 0xff, back_color & 0xff);
	fore = gdImageColorAllocate(im, (fore_color >> 16) & 0xff, (fore_color >> 8) & 0xff, fore_color & 0xff);

	glyph[0] = start_char;
	glyph[1] = 0;

	for (chr = 0; chr < num_of_glyphs; chr++) {
		if ((err_str = gdImageStringFT(NULL, &b_rect[0], fore, path_to_font, (double)font_size, 0.0, 0, 0, glyph)) != NULL) {
			fprintf(stderr, "%s\n", err_str);
			exit(1);
		}

		if (baseline > b_rect[5]) {
			baseline = b_rect[5];
		}

		if (baseline > b_rect[7]) {
			baseline = b_rect[7];
		}

		glyph[0]++;
	}

	if (baseline < 0) {
		baseline = -baseline;
	}

	glyph[0] = start_char;
	
	for (chr = 0; chr < num_of_glyphs; chr++) {
		if ((err_str = gdImageStringFT(NULL, &b_rect[0], fore, path_to_font, (double)font_size, 0.0, 0, 0, glyph)) != NULL) {
			fprintf(stderr, "%s\n", err_str);
			exit(1);
		}

		opt = 0;
		if (opt > b_rect[0]) {
			opt = b_rect[0];
		}

		if (opt > b_rect[6]) {
			opt = b_rect[6];
		}

		opt = -opt;
		/*
		if (x == 0) {
			printf("%c: %i (%i %i %i %i %i %i %i %i)\n", glyph[0], opt + x, bRect[0], bRect[1], bRect[2], bRect[3], bRect[4], bRect[5], bRect[6], bRect[7]);
		}
		*/

		if ((err_str = gdImageStringFT(im, &b_rect[0], fore, path_to_font, (double) font_size, 0.0, x + opt, y + baseline, glyph)) != NULL) {
			fprintf(stderr, "%s\n", err_str);
			exit(1);
		}

		if (glyph[0] == ' ') {
			opt = size_of_space;
		} else {
			opt = b_rect[2] - b_rect[0];
			tmp = b_rect[4] - b_rect[6];
			if (opt < tmp) {
				opt = tmp;
			}
		}

		fprintf(f, "%i", opt);

		idx++;
		if (idx == glyphs_pr_line) {
			idx = 0;
			x = 0;
			y += glyph_size;
			fprintf(f, ",\n");
		} else {
			x += glyph_size;
			fprintf(f, ", ");
		}

		glyph[0]++;
	}

	fclose(f);

/*
 * bRect:
 * 0	lower left corner, X position
 * 1	lower left corner, Y position
 * 2	lower right corner, X position
 * 3	lower right corner, Y position
 * 4	upper right corner, X position
 * 5	upper right corner, Y position
 * 6	upper left corner, X position
 * 7	upper left corner, Y position
 */
	png_out = fopen(font_out, "wb");
	gdImagePng(im, png_out);
	fclose(png_out);

	gdImageDestroy(im);

	exit(0);
}
