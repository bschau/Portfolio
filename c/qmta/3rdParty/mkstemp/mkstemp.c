/*******************************************************************************
 *
 *	This program is part of the QMTA package.
 *
 *	- compile and install this program before installing QMTA.
 *
 *	This program (c) 1999- by Brian Schau.
 */
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* globals */
extern int errno;

/* protos */
static void usage(char *);

/*******************************************************************************
 *
 *	usage
 *
 *	Write friendly usage greeting.
 *
 *	Input:
 *		pn - påprogramname.
 */
static void
usage(char *pn)
{
	fprintf(stderr, "Usage: %s [-h] [-q] filename\n\n", pn);
	fprintf(stderr, "  -h - help screen (this screen)\n");
	fprintf(stderr, "  -q - suppress errors\n");
	fprintf(stderr, "%s creates a unique temporary file.  The name is written to stdout\n", pn);
	fprintf(stderr, "\nnotes on filename:\n");
	fprintf(stderr, "- trailing XXXXXX's is appended\n");
	fprintf(stderr, "- if a slash is first char, the filename is taken to be an absolute path,\n  otherwise honour TMPDIR if set, otherwise create in current direcory\n");
}

/*******************************************************************************
 *
 *	main
 */
int
main(int argc,
     char *argv[])
{
	int noisy=1, err, opt, tplen, fd;
	char *fn, *argp, *tp;

	while ((opt=getopt(argc, argv, "hq"))!=-1) {
		switch (opt) {
			case 'q':
				noisy=0;
				break;
			case 'h':
				usage(argv[0]);
				break;
			default:
				usage(argv[0]);
				exit(1);
		}
	}

	if ((argc-optind)!=1) {
		usage(argv[0]);
		exit(1);
	}

	argp=argv[optind];

	if (*argp=='/') {	/* absolute path */
		opt=strlen(argp);
		fn=(char *)calloc(opt+8, 1);	/* ".XXXXXX\0" */
		if (!fn) {
			if (noisy)
				fprintf(stderr, "cannot allocate %i bytes\n", opt+8);
			exit(1);
		}

		memmove(fn, argv[optind], opt);
	} else {
		opt=strlen(argp);
		tp=getenv("TMPDIR");
		if (tp) {
			tplen=strlen(tp);
			if (*(tp+tplen-1)=='/') {
				tplen--;
				if (!tplen)
					tplen=1;
			}
		} else
			tplen=0;

		fn=(char *)calloc(opt+tplen+9, 1);	/* ".XXXXXX\0" & '/' */
		if (!fn) {
			if (noisy)
				fprintf(stderr, "cannot allocate %i bytes\n", opt+8);
			exit(1);
		}

		err=0;
		if (tplen) {
			memmove(fn, tp, tplen);
			*(fn+tplen)='/';
			err=tplen+1;
		}

		memmove(fn+err, argp, opt);
		opt+=err;
	}

	memmove(fn+opt, ".XXXXXX", 7);

	fd=mkstemp(fn);
	if (fd==-1) {
		if (noisy)
			fprintf(stderr, "%s\n", strerror(errno));
		err=1;
	} else {
		close(fd);
		printf("%s\n", fn);
		err=0;
	}

	free(fn);
	exit(err);
}
