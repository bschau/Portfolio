#include "private.h"

/* macros */
#define IPADDRLEN 32	/* enough for dotted quad */

/*******************************************************************************
*
*	spamcheck
*
*	Master spam check routine.
*
*	Input:
*		ip - ipaddress.
*		verb - verbose (1) or silent (0).
*	Output:
*		host not listed: SCINNOCENT.
*		host listed: SCSPAMMER.
*		cannot decide (some error): SCPARK.
*/
int
spamcheck(char *ip,
          int verb)
{
	int len=strlen(ip), status=SCINNOCENT;
	char rotip[IPADDRLEN+2];
	char *ptr;
	char tmp;

	if (!spamcheckact) {
		if (verb)
			fprintf(stderr, "spamcheck is turned off\n");

		return SCINNOCENT;	/* exit */
	}

	/* rotate ip-address */
	ip+=len;
	ptr=rotip;
	for (; len>0; len--)
		*ptr++=*--ip;

	*ptr++='.';
	*ptr='\x00';

	for (ptr=rotip; *ptr; ) {
		if (*(ptr+1)=='.') {
			/* no need for rotation */
			ptr+=2;
			continue;
		}

		if (*(ptr+2)=='.') {
			tmp=*(ptr+1);
			*(ptr+1)=*ptr;
			*ptr=tmp;
			ptr+=3;
			continue;
		}

		if (*(ptr+3)=='.') {
			tmp=*(ptr+2);
			*(ptr+2)=*ptr;
			*ptr=tmp;
			ptr+=4;
			continue;
		}

		if (verb)
			fprintf(stderr, "malformed ip address\n");

		return SCPARK;	/* exit */
	}

	status|=rbl(ip, rotip, verb);

	return status;
}
