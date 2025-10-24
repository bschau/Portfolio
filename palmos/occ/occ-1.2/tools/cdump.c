#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* macros */
enum { FormatPalm, FormatOther };

/* protos */
static int Dump(char *, FILE *, FILE *, int);

/*
**	main
*/
int
main(int argc,
     char *argv[])
{
	FILE *ifh=NULL, *ofh=NULL;
	int format;

	if (argc!=5) {
		fprintf(stderr, "Usage: cdump format name in-file c-file\n");
		fprintf(stderr, "\nFormat can be either Palm or Other\n");
		exit(1);
	}

	if (strcasecmp(argv[1], "palm")==0)
		format=FormatPalm;
	else
		format=FormatOther;

	ifh=fopen(argv[3], "rb");
	ofh=fopen(argv[4], "wb+");
	if (ifh && ofh) {
		if (Dump(argv[2], ifh, ofh, format))
			fprintf(stderr, "failed ...\n");
	} else
		fprintf(stderr, "cannot open files\n");

	if (ofh)
		fclose(ofh);

	if (ifh)
		fclose(ifh);

	exit(0);
}

/*
**	Dump
**
**	Dumps from in to out.
**
**	 -> name	Name of variable.
**	 -> ifh		Input file handle (prc file).
**	 -> ofh		Output file handle (c file).
**	 -> format	Format of output (FormatPalm or FormatOther).
**	<-  error	0 - succes, 1 - failure.
*/
static int
Dump(char *name, FILE *ifh, FILE *ofh, int format)
{
	long idx=0, len;
	int c;

	if (fseek(ifh, 0, SEEK_END)) {
		fprintf(stderr, "cannot seek input file\n");
		return 1;
	}

	len=ftell(ifh);
	rewind(ifh);

	if (format==FormatPalm) {
		fprintf(ofh, "UInt32 %sLen=%li;\n\n", name, len);
		fprintf(ofh, "UInt8 %sArr[%li]={\n\t", name, len);
	} else {
		fprintf(ofh, "unsigned long %sLen=%li;\n\n", name, len);
		fprintf(ofh, "unsigned char %sArr[%li]={\n\t", name, len);
	}
	for (; len>0; len--) {
		if ((c=fgetc(ifh))==EOF) {
			fprintf(stderr, "sudden end of file in input file\n");
			return 1;
		}

		fprintf(ofh, "0x%02X", (unsigned char)c);

		if (len>1)
			fprintf(ofh, ", ");

		if ((idx%11)==10)
			fprintf(ofh, "\n\t");

		idx++;
	}
	fprintf(ofh, "\n};\n");

	return 0;
}
