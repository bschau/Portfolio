#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAIN
#include "common.h"
#include "bigfont.h"

#define SCREEN_X 80
#define S_WIDTH (SCREEN_X + 1)

/* forward decls */
static void usage(int ec);
static void make_letter(unsigned int gx_offset, unsigned char *arr_ptr);
static void make_scanline(unsigned char byte, unsigned char *arr_ptr);

/* globals */
static unsigned char gfx_char = '#';

/**
 * Usage
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "big %s\n", bstools_version);
	fprintf(f, "Usage: big [OPTIONS] 'text' 'text' ...\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -c       Center text\n");
	fprintf(f, "  -g char  Character to use\n");
	fprintf(f, "  -s       Use letters character\n");

	exit(ec);
}

/*
 * Main.
 */
int main(int argc, char *argv[]) {
	int center = 0, self = 0, arg_cnt, offset, fnt_siz, chr, end_chr;
	char *this_arg;
	unsigned char *arr_ptr;
	unsigned char array[(S_WIDTH * FONT_Y) + 2];
	unsigned char pre[S_WIDTH];

	while ((chr = getopt(argc, argv, "cg:s")) != -1) {
		switch (chr) {
			case 'c':
				center = 1;
				break;

			case 'g':
				gfx_char = *optarg;
				break;

			case 's': 
				self = 1;
				break;

			default:
				fprintf(stderr, "Unknown option '%c'\n", chr);
				usage(1);
		}
	}

	if (optind == argc) {
		usage(0);
	}

	for (arg_cnt = optind; arg_cnt < argc; arg_cnt++) {
		this_arg = argv[arg_cnt];
		offset = 0;
		
		while (*this_arg) {
			arr_ptr = array + offset;
			if (*this_arg == '\\') {
				this_arg++;
				if (*this_arg == '\\') {
					chr = (int) '\\';
				} else {
					chr = (int) ((*this_arg++) - '0') * 64;
					chr += (int) ((*this_arg++) - '0') * 8;
					chr += (int) (*this_arg) - '0';
				}
			} else {
				chr = (int) *this_arg;
			}

			if (self) {
				gfx_char = (unsigned char) chr;
			}

			chr *= FONT_Y;
			make_letter(chr, arr_ptr);
			offset += 8;
			if (offset >= SCREEN_X) {
				offset = SCREEN_X;
				break;
			}

			this_arg++;
		}
		
		end_chr = (offset >= SCREEN_X) ? '\x00' : '\n';

		for (fnt_siz = 0; fnt_siz < FONT_Y; fnt_siz++) {
			array[offset] = '\x00';
			offset += S_WIDTH;
		}
		
		if (center) {
			chr = (SCREEN_X - strlen((const char *) array)) / 2;
			for (offset = 0; offset < chr; offset++) {
				pre[offset] = ' ';
			}

			pre[chr] = '\x00';
		} else {
			*pre = '\x00';
		}

		for (fnt_siz = 0; fnt_siz < FONT_Y; fnt_siz++) {
			printf("%s%s%c", pre, &array[fnt_siz * S_WIDTH], end_chr);
		}
	}

	exit(0);
}

/**
 * Blit letter to array.
 *
 * @param gfx_offset Offset into array.
 * @param arr_ptr Array.
 */
static void make_letter(unsigned int gfx_offset, unsigned char *arr_ptr) {
	int fnt_siz;

	for (fnt_siz = 0; fnt_siz < FONT_Y; fnt_siz++) {
		make_scanline(font_gfx[gfx_offset], arr_ptr);
		gfx_offset++;
		arr_ptr += S_WIDTH;
	}
}


/**
 * Create one scanline of the letter.
 *
 * @param byte Character.
 * @param arr_ptr Array.
 */
static void make_scanline(unsigned char byte, unsigned char* arr_ptr) {
	arr_ptr[0] = (byte & 0x80) ? gfx_char : ' ';
	arr_ptr[1] = (byte & 0x40) ? gfx_char : ' ';
	arr_ptr[2] = (byte & 0x20) ? gfx_char : ' ';
	arr_ptr[3] = (byte & 0x10) ? gfx_char : ' ';
	arr_ptr[4] = (byte & 0x08) ? gfx_char : ' ';
	arr_ptr[5] = (byte & 0x04) ? gfx_char : ' ';
	arr_ptr[6] = (byte & 0x02) ? gfx_char : ' ';
	arr_ptr[7] = (byte & 0x01) ? gfx_char : ' ';
}
