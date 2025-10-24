#include "private.h"

/* protos */
static int localdns(void);
static int configuration(void);

/*******************************************************************************
*
*	localdns
*
*	Finds out what our local name and domain is set up like.
*
*	Output:
*		s: 0.
*		f: !0.
*/
static int
localdns(void)
{
	int ret=0, len;
	char name[DNSPARTLEN+2];

	if (relayname)	/* override check */
		return 0;

	memset(name, 0, DNSPARTLEN+1);
	gethostname(name, DNSPARTLEN);
	len=strlen(name);
	if (!len) {
		fprintf(stderr, "error: could not guess hostname\n");
		ret=1;
	}
	
	memset(name, 0, DNSPARTLEN+1);
	getdomainname(name, DNSPARTLEN);
	len=strlen(name);
	if (!len) {
		fprintf(stderr, "error: could not guess domainname\n");
		ret=1;
	}

	return ret;
}

/*******************************************************************************
*
*	configuration
*
*	Check some/all of the configuration directives for missing or
*	invalid values.
*
*	Output:
*		s: 0.
*		f: !0.
*/
static int
configuration(void)
{
	int ret=0;

	if (!localmailto) {
		fprintf(stderr, "error: localmailto not set\n");
		ret=1;
	}

	if (!smaxsweep) {
		fprintf(stderr, "warning: sweepalarm was 0, set to 300\n");
		smaxsweep=300;
	}

	return ret;
}

/*******************************************************************************
*
*	sanity
*
*	Do some basic sanity checks.   Squeal if something is wrong.
*/
void
sanity(void)
{
	int ret=0;

	ret|=configuration();
	ret|=localdns();

	if (ret)	/* abort? */
		exit(1);
}
