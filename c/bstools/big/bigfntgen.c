/**
 * Convert mode 0 or mode 1 psf1 fonts to a .h file readable by big.c
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * Main.
 */
int main(int argc, char* argv[]) {
	FILE *in, *out;
	int fm, h, cnt, chr;

	if (argc != 3) {
		fprintf(stderr, "Usage: bigfntgen in out\n");
		exit(1);
	}

	if ((in = fopen(argv[1], "rb")) == NULL) {
		perror(argv[1]);
		exit(1);
	}

	if ((out = fopen(argv[2], "wb")) == NULL) {
		perror(argv[2]);
		exit(1);
	}

	if ((chr = fgetc(in)) != 0x36) {
		fprintf(stderr, "Not a psf1 file\n");
		exit(1);
	}

	if ((chr = fgetc(in)) != 0x04) {
		fprintf(stderr, "Not a psf1 file\n");
		exit(1);
	}

	switch (fgetc(in)) {
		case 0:
			fm = 256;
			break;

		case 1:
			fm = 512;
			break;

		default:
			fprintf(stderr, "Not a mode 0 or mode 1 psf1 file\n");
			exit(1);
	}

	h = fgetc(in);

	fprintf(out, "#ifndef FONT_H\n#define FONT_H 1\n\n#define FONT_Y 16\n#define FONT_WIDTH %i\n\nunsigned char font_gfx[] = \\\n", fm);

	for (cnt = 0; cnt < (fm * h) / 16; cnt++) {
		fprintf(out, "\t\"");
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\\x%02X", fgetc(in));
		fprintf(out, "\" \\\n");
	}

	fprintf(out, ";\n\n#endif\n");
	fclose(out);
	fclose(in);

	exit(0);
}
