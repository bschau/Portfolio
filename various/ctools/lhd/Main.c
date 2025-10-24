/**
 * Main.c
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Version.h"

/* protos */
static void Version(FILE *);
static int Usage(int);
static void Xxd(FILE *, off_t, off_t);
static void Parse(char *, off_t *, off_t *);
static off_t Convert(char *);

/* globals */
static struct stat sBuf;
static char *file;

/**
 * Output version string.
 *
 * @param dst Destination channel.
 */
static void
Version(FILE *dst)
{
	fprintf(dst, "lhd from ltools-%s\n", VERSION);
}

/**
 * Output usage information.
 *
 * @param ex Exit code.
 */
static int
Usage(int ex)
{
	FILE *dst;

	if (ex) {
		dst=stderr;
	} else {
		dst=stdout;
	}

	Version(dst);
	fprintf(dst, "Usage: lhd file [ranges]\n\n");
	fprintf(dst, "Ranges can be given as:\n\n");
	fprintf(dst, "  r1:  or\n");
	fprintf(dst, "  r1       Show from 'r1' to the end of the file\n");
	fprintf(dst, "  :r2  or\n");
    	fprintf(dst, "  +r2      Show from the start of the file to 'r2'\n");
	fprintf(dst, "  r1:r2    Show from 'r1' to 'r2'\n");
	fprintf(dst, "  r1+r2    Show from 'r1' and 'r2' bytes onwards\n\n");
	fprintf(dst, "Multiple ranges must be given - they should be separated by whitespaces\n");
	fprintf(dst, "Values can be prefixed with:\n\n");
	fprintf(dst, "  0x1234 or\n");
	fprintf(dst, "  x1234    Hexadecimal value\n");
	fprintf(dst, "  01234    Octal value\n");
	fprintf(dst, "  %%1010    Binary value\n\n");
	fprintf(dst, "No prefix means plain old decimal system\n");
	exit(ex);
}

/**
 * Main.
 */
int
main(int argc, char *argv[])
{
	FILE *fp;
	int aIdx;
	off_t s, e;

	if (argc<2) {
		Usage(1);
	}

	file=argv[1];
	if (stat(file, &sBuf)) {
		perror(file);
		exit(1);
	}

	if ((fp=fopen(file, "rb"))==NULL) {
		perror(file);
		exit(1);
	}

	if (argc==2) {
		Xxd(fp, 0, sBuf.st_size);
	} else {
		for (aIdx=2; aIdx<argc; aIdx++) {
			Parse(argv[aIdx], &s, &e);
			Xxd(fp, s, e);
		}
	}

	fclose(fp);

	exit(0);
}

/**
 * Dump content of file.
 *
 * @param fp FILE pointer.
 * @param beg Start.
 * @param end End.
 */
static void
Xxd(FILE *fp, off_t beg, off_t end)
{
	off_t pos=beg, left, this, i, idx, blanks;
	unsigned char c[16];

	if (beg>sBuf.st_size) {
		beg=0;
	}

	if (end>sBuf.st_size) {
		end=0;
	}

	if (beg>end) {
		off_t t=end;

		end=beg;
		beg=t;
	}

	if (fseek(fp, beg, SEEK_SET)) {
		perror(file);
		exit(1);
	}

	left=end-beg;
	while (left) {
		printf("%07x: ", (int)pos);
		this=(left>15) ? 16 : left;
		if (fread(c, 1, this, fp)!=this) {
			perror(file);
			exit(1);
		}

		idx=0;
		blanks=40;
		for (i=0; i<(this/2); i++) {
			printf("%02x%02x ", c[idx], c[idx+1]);
			idx+=2;
			blanks-=5;
		}

		if (this&1) {
			printf("%02x   ", c[idx]);
			blanks-=5;
		}

		for (i=0; i<blanks; i++) {
			printf(" ");
		}

		printf(" ");
		for (i=0; i<this; i++) {
			if ((c[i]<' ') || (!isprint(c[i]))) {
				printf(".");
			} else {
				printf("%c", c[i]);
			}
		}

		pos+=this;
		left-=this;

		printf("\n");
	}
}

/**
 * Parse an argument into start and end index.
 *
 * @param arg Argument.
 * @param s Start index.
 * @param e End index.
 */
static void
Parse(char *arg, off_t *s, off_t *e)
{
	char *p;

	if ((p=strchr(arg, ':'))) {
		/* range */
		*p=0;
		*s=Convert(arg);
		*e=Convert(p+1);
		return;
	} else if ((p=strchr(arg, '+'))) {
		/* offset */
		if (p==arg) {
			*s=0;
		} else {
			*p=0;
			*s=Convert(arg);
		}

		*e=*s+Convert(p+1);
		return;
	}

	*s=Convert(arg);
	*e=sBuf.st_size;
}

/**
 * Convert a string to a off_t value.
 *
 * @param src Source string.
 * @return off_t.
 */
static off_t
Convert(char *src)
{
	char *save=src;
	char c;
	off_t v=0;

	if (!*src) {
		return v;
	}

	if (*src=='0') {
		src++;
		if (toupper(*src)!='X') {
			/* octal */
			while (*src) {
				if ((*src<'0') || (*src>'7')) {
					fprintf(stderr, "Unknown digit in octal '%s'\n", save);
					exit(1);
				}

				v*=8;
				v+=*src-'0';
				src++;
			}
			return v;
		}
	}

	if (toupper(*src)=='X') {
		src++;
		/* hex */
		while (*src) {
			c=toupper(*src);
			if (((c>='0') && (c<='9'))
			   || ((c>='A') && (c<='F'))) {
				v*=16;
				v+=(((c>='0') && (c<='9')) ? (c-'0') : (c-'A'+10));
				src++;
			} else {
				fprintf(stderr, "Unknown digit in hexadecimal '%s'\n", save);
				exit(1);
			}
		}
		return v;
	}

	if (*src=='%') {
		src++;
		while (*src) {
			v<<=1;
			if (*src=='0') {
				v&=~1;
			} else if (*src=='1') {
				v|=1;
			} else {
				fprintf(stderr, "Unknown digit in binary '%s'\n", save);
				exit(1);
			}

			src++;
		}
		/* binary */
		return v;
	}

	/* decimal */
	while (*src) {
		if ((*src<'0') || (*src>'9')) {
			fprintf(stderr, "Unknown digit in decimal '%s'\n", save);
			exit(1);
		}

		v*=10;
		v+=*src-'0';
		src++;
	}

	return v;
}
