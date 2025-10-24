#include "private.h"

/* protos */
static int rblcheck(char *, int);

/*******************************************************************************
*
*	rbl
*
*	Check against mail-abuse.org types.
*
*	Input:
*		ip - ip-address.
*		rotid - rotated ip-address.
*		verb - verbose (1) or silent (0).
*	Output:
*		host is listed: SCSPAMMER.
*		host is not listed: SCINNOCENT.
*		some sort of error: SCPARK.
*/
int
rbl(char *ip,
    char *rotip,
    int verb)
{
	int len=HUGELINELEN;

	if (!rblhost) {
		if (verb)
			fprintf(stderr, "no rbl host\n");
		else
			qlog(QINFO, "(rbl) no rbl host");

		return SCPARK;	/* exit */
	}

	memset(line, 0, HUGELINELEN+1);
	strncpy(line, rotip, len);
	len-=strlen(rotip);
	if (len>0) {
		strncat(line, rblhost, len);
		return rblcheck(ip, verb);
	}

	if (verb)
		fprintf(stderr, "cannot form name - buffer would overflow\n");
	else
		qlog(QINFO, "(rbl) name too long");

	return SCPARK;
}
/*******************************************************************************
*
*	rblcheck
*
*	Check against mail-abuse.org types.
*
*	Input:
*		ip - ip-address.
*		verb - verbose (1) or silent (0).
*	Output:
*		host is listed: SCSPAMMER.
*		host is not listed: SCINNOCENT.
*		some sort of error: SCPARK.
*/
int
rblcheck(char *ip,
         int verb)
{
	unsigned int ipaddr;
	int ret;
	struct hostent *he;
	char **addr;

	if (verb)
		printf("Checking for %s ('%s'): ", ip, line);

	endhostent();
	he=gethostbyname(line);
	if (!he) {
		if (h_errno==HOST_NOT_FOUND || h_errno==NO_DATA) {
			if (verb)
				printf("not listed\n");

			return SCINNOCENT;
		}

		if (verb)
			printf("failed (%i)\n", h_errno);
		else
			qlog(QINFO, "(rblcheck) gethostbyname failed, h_errno is %i", h_errno);

		return SCPARK;
	}

	addr=he->h_addr_list;
	ipaddr=ntohl((int)**addr);
	if (ipaddr==2130706432)		/* 127.0.0.2 */
		ret=SCSPAMMER;
	else
		ret=SCINNOCENT;

	if (verb)
		printf("spammer (%s)\n", inet_ntoa(*(struct in_addr *)*addr));

	return ret;
}
