#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAIN
#include "occmake.h"

/* protos */
static void ExitHandler(void);
static void SignalHandler(int);
static int Usage(int);
static void Initialize(char *, char *, char *, char *);

/* globals */
extern unsigned long largeLen, smallLen;
extern unsigned char largeArr[], smallArr[];

/*
**	ExitHandler
**
**	Cleanup handler.
*/
static void
ExitHandler(void)
{
	if (prcOk==0)
		unlink(fileName);

	if (inMemPrc)
		free(inMemPrc);

	if (mergedSmallBitmap)
		free(mergedSmallBitmap);

	if (mergedLargeBitmap)
		free(mergedLargeBitmap);

	if (smallBitmap)
		free(smallBitmap);

	if (largeBitmap)
		free(largeBitmap);

	if (fileName)
		free(fileName);

	if (url)
		free(url);
}

/*
**	SignalHandler
**
**	Handle signal.
*/
static void
SignalHandler(int sigNum)
{
	fprintf(stderr, "caught signal %i\n", sigNum);
	exit(0);
}

/*
**	Usage
**
**	Write usage to stdout.
**
**	 -> errCode	Errorcode to exit with.
*/
static int
Usage(int errCode)
{
	fprintf(stderr, "Usage:\n
    occmake [options] occ-name url\n
Options:\n
-e           Make standalone executable.
-h           This text.
-l bmpname   BMP file to use for large icon.
-n prcname   Name of OCC in database (ie. in launcher).
-s bmpname   BMP file to use for small icon.\n");
	exit(errCode);
}

/*
**	main
*/
int
main(int argc, char *argv[])
{
	char *lBmp=NULL, *sBmp=NULL, *nameInPrc=NULL, *prcName, *userUrl;
	int makeExecutable=0, opt;

	while ((opt=getopt(argc, argv, "ehl:n:s:"))!=-1) {
		switch (opt) {
		case 'e':
			makeExecutable=1;
			break;
		case 'h':
			Usage(0);
			break;
		case 'l':
			lBmp=optarg;
			break;
		case 'n':
			nameInPrc=optarg;
			break;
		case 's':
			sBmp=optarg;
			break;
		default:
			Usage(1);
			break;
		}
	}
		
	if ((argc-optind)!=2)
		Usage(1);

	prcName=argv[optind++];
	userUrl=argv[optind];

	if (atexit(ExitHandler)) {
		fprintf(stderr, "cannot install atexit-handler\n");
		exit(1);
	}

	signal(SIGINT, SignalHandler);
	signal(SIGHUP, SignalHandler);
	signal(SIGQUIT, SignalHandler);
	signal(SIGTERM, SignalHandler);

	Initialize(prcName, userUrl, lBmp, sBmp);

	prcOk=0;
	BuildPrc(prcName, nameInPrc, makeExecutable);
	prcOk=1;

	exit(0);
}

/*
**	Initialize
**
**	Load default stuff.
**
**	 -> name	Name of OCC.
**	 -> userurl	Url (as given by user).
**	 -> large	Large bitmap file.
**	 -> small	Small bitmap file.
*/
static void
Initialize(char *name, char *userurl, char *large, char *small)
{
	/* file name */
	if ((fileName=XConCat(name, ".prc"))==NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}

	/* url */
	if (strncasecmp(userurl, "http://", 7)==0)
		url=strdup(userurl);
	else
		url=XConCat("http://", userurl);

	if (url==NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}

	/* bitmaps */
	BitmapLoad(large, &mergedLargeBitmap, &mergedLargeBitmapLen, largeArr, largeLen);
	BitmapLoad(small, &mergedSmallBitmap, &mergedSmallBitmapLen, smallArr, smallLen);
}
