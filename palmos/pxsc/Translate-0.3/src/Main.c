/**
 * Main.c
 */
#include <stdio.h>
#include <unistd.h>
#define MAIN
#include "Include.h"

/* protos */
static void Version(FILE *);
static int Usage(int);
static char *LoadFile(char *);
static HashTable LoadLanguage(char *);
static void StrToLower(char *);

/* globals */
extern int optIndex;
extern char optError[];
extern char *optArg;


/**
 * Output version string.
 *
 * @param dst Destination channel.
 */
static void
Version(FILE *dst)
{
	fprintf(dst, "Translate v0.3\n");
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

	if (ex)
		dst=stderr;
	else
		dst=stdout;

	Version(dst);
	fprintf(dst, "Usage: translate [OPTIONS] template-file language-file\n\n");
	fprintf(dst, "[OPTIONS]\n");
	fprintf(dst, "  -C token           Closing token (defaults to @@)\n");
	fprintf(dst, "  -O token           Opening token (defaults to @@)\n");
	fprintf(dst, "  -V key=val         Define key = value\n");
	fprintf(dst, "  -d language-file   Set the base language to be overlaid\n");
	fprintf(dst, "  -h                 Show help (this page)\n");
	fprintf(dst, "  -o file            Output to 'file'\n");
	exit(ex);
}

/**
 * Main.
 */
int
main(int argc, char *argv[])
{
	char *loadDefault=NULL, *outputName=NULL, *tmpl, *src, *t, *data;
	char *line;
	FILE *fOut=NULL, *fDst=NULL;
	int lineNo=0, opt, idx;

	OptionInit();

	while ((opt=OptionNext(argc, argv, "C:O:d:ho:V:"))!=-1) {
		switch (opt) {
		case 'C':
			tClosing=optArg;
			break;
		case 'O':
			tOpening=optArg;
			break;
		case 'V':
			if (!htCli) {
				if ((htCli=HashNew(6))==NULL) {
					fprintf(stderr, "Failed to allocate Hash table\n");
					exit(1);
				}
			}

			src=optArg;
			for (tmpl=src; *tmpl && *tmpl!='='; tmpl++);
			if (*tmpl=='=') {
				*tmpl++=0;
				if (*tmpl) {
					if ((tmpl=strdup(tmpl))==NULL) {
						fprintf(stderr, "Out of memory\n");
						exit(1);
					}

					StrToLower(src);
					if (HashAdd(htCli, src, tmpl, strlen(tmpl), HTFreeValue)) {
						fprintf(stderr, "Failed to add '%s' to the Hash table", src);
						exit(1);
					}
				}
			}
			break;
		case 'd':
			loadDefault=optArg;
			break;
		case 'h':
			Usage(0);
		case 'o':
			outputName=optArg;
			break;
		default:
			fprintf(stderr, "%s\n", optError);
			Usage(1);
		}
	}

	if ((argc-optIndex)!=2)
		Usage(1);

	if (loadDefault)
		htBase=LoadLanguage(loadDefault);

	htLang=LoadLanguage(argv[optIndex+1]);

	tmpl=LoadFile(argv[optIndex]);

	if (outputName) {
		if ((fOut=fopen(outputName, "wb"))==NULL) {
			perror(outputName);
			exit(1);
		}

		fDst=fOut;
	} else
		fDst=stdout;

	tOpeningLen=strlen(tOpening);
	tClosingLen=strlen(tClosing);
	src=tmpl;
	for (EVER) {
		lineNo++;
		line=src;
		for (; *src && *src!='\n'; src++);
		if (!*src)
			break;

		*src++=0;

		opt=1;
		while (opt) {
			if ((t=(char *)strstr(line, tOpening))) {
				if (t-line>0) {
					*t=0;
					fprintf(fDst, "%s", line);
				}

				line=t+tOpeningLen;
				if ((t=(char *)strstr(line, tClosing))) {
					*t=0;
					StrToLower(line);
					if ((data=HashGet(htCli, line, NULL))==NULL) {
						if ((data=HashGet(htLang, line, NULL))==NULL) {
							if (htBase) {
								fprintf(stderr, "%i:Value for token '%s' in language file not found\n", lineNo, line);
								if ((data=HashGet(htBase, line, NULL))==NULL) {
									fprintf(stderr, "%i:Value for token '%s' not found in default overlay either\n", lineNo, line);
									exit(1);
								}
							} else {
								fprintf(stderr, "%i:Unknown token '%s'\n", lineNo, line);
								exit(1);
							}
						}
					}

					fprintf(fDst, "%s", data);
					line=t+tClosingLen;
				} else {
					fprintf(stderr, "Opening sequence with no closing found on line %i\n", lineNo);
					fprintf(fDst, "%s", tOpening);
				}
			} else {
				if (*line)
					fprintf(fDst, "%s", line);

				opt=0;
			}
		}

		fprintf(fDst, "\n");
	}

	if (fOut)
		fclose(fOut);

	exit(0);
}

/**
 * Load the given file.
 *
 * @param f File to load.
 * @return Zero terminated string with source.
 * @note Will exit in case of failure.
 */
static char *
LoadFile(char *f)
{
	FILE *fh;
	long size;
	char *s;

	if (access(f, R_OK)) {
		perror(f);
		exit(1);
	}

	if ((fh=fopen(f, "rb"))==NULL) {
		perror(f);
		exit(1);
	}

	if (fseek(fh, 0, SEEK_END)) {
		perror(f);
		exit(1);
	}

	size=ftell(fh);
	rewind(fh);

	if ((s=(char *)malloc(size+1))==NULL) {
		fprintf(stderr, "Failed to allocate %i bytes\n", size+1);
		exit(1);
	}

	if (fread(s, size, 1, fh)!=1) {
		perror(f);
		exit(1);
	}

	s[size]=0;
	fclose(fh);

	return s;
}

/**
 * Load a language into a hash table.
 *
 * @param file Language file name.
 * @return HashTable or NULL.
 */
static HashTable
LoadLanguage(char *file)
{
	char *src=LoadFile(file), *s;
	HashTable ht=HashNew(10);
	StringBuilder sb=SBNew(512);
	int lineNo=1, len;
	char *token, *data;

	if (!ht) {
		fprintf(stderr, "Failed to allocate hashtable\n");
		exit(1);
	}

	s=src;
	while (*s) {
		for (; *s && isspace((int)*s); s++) {
			if (*s=='\n')
				lineNo++;
		}

		if (!*s)
			break;

		if (*s=='#') {
			if ((s[1]=='e') && (s[2]=='o') && (s[3]=='f'))
				break;

			for (; *s && *s!='\n'; s++);
		} else {
			token=s;
			for (; *s && (isalpha((int)*s) || isdigit((int)*s)); s++);
			if (!*s) {
				fprintf(stderr, "%i:Missing data to keyword '%s'\n", lineNo, token);
				exit(1);
			}

			*s++=0;
			SBReset(sb);

			for (; *s && *s=='\t'; s++);
			if (!*s) {
				fprintf(stderr, "%i:Missing data to keyword '%s'\n", lineNo, token);
				exit(1);
			}

			for (EVER) {
				data=s;
				for (; *s && *s!='\n'; s++) {
					if (*s=='\r') {
						SBAppend(sb, data, s-data);
						data=s+1;
					}
				}

				if (s-data)
					SBAppend(sb, data, s-data);

				if (!*s)
					break;

				lineNo++;
				s++;
				if (*s!='\t')
					break;

				for (; *s && *s=='\t'; s++);
				if (!*s)
					break;
			}

			len=SBLength(sb);
			if ((data=(char *)malloc(len+1))==NULL) {
				fprintf(stderr, "Failed to allocated %i bytes for %s\n", len+1, token);
				exit(1);
			}

			memmove(data, SBToString(sb), len);
			data[len]=0;

			StrToLower(token);
			if (HashAdd(ht, token, data, len+1, HTFreeValue)) {
				fprintf(stderr, "Failed to add '%s' to the hashtable\n", token);
				exit(1);
			}
		}
	}

	free(src);
	SBFree(sb);

#if 0
	{
		/* Dump the HashTable to stdout */
		const char *k;
		const void *v;
		int s;

		if (HashFindFirst(ht, &k, &v, &s)==0) {
			do {
				printf("'%s' => '%s'\n", k, (char *)v);
			} while (!HashFindNext(ht, &k, &v, &s));
		}
	}
#endif
	return ht;
}

/**
 * Convert a string to lower case.
 *
 * @param src String to be converted.
 */
static void
StrToLower(char *src)
{
	char *dst=src;

	while (*src) {
		if (*src!='\r')
			*dst++=tolower(*src);

		src++;
	}

	*dst=0;
}
