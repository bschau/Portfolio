#include "private.h"

/* macros */
#define MPSARGS " -fq ../"
#define MPSARGSLEN strlen(MPSARGS)

/*******************************************************************************
*
*	munpack
*
*	Detach attachments using the munpack program.
*
*	Input:
*		cntl - control file name.
*	Output:
*		s: 0.
*		f: !0.
*/
int
munpack(char *cntl)
{
	char mailfile[CNTLFILENAMELEN+1];
	char *mps;
	int err;

	/* build command string */
	mps=(char *)calloc(strlen(munpackbin)+MPSARGSLEN+CNTLFILENAMELEN+1, 1);
	if (!mps) {
		qlog(QWARN, "(munpack) cannot allocate mps");
		return 1;
	}

	memmove(mailfile+1, cntl+1, CNTLFILENAMELEN-1);
	*mailfile='m';
	mailfile[CNTLFILENAMELEN]='\x00';

	strcpy(mps, munpackbin);
	strcat(mps, MPSARGS);
	strcat(mps, mailfile);
	err=system(mps);
	if (err==-1 || err==127) {
		qlog(QWARN, "(munpack) cannot run %s (%i)", mps, errno);
		free(mps);
		return 1;
	}
	free(mps);

	return 0;
}
