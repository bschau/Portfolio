#include "private.h"

/* globals */
static int lineno;

/* protos */
static int parse(FILE *file);
static void parseline(char *);
static bit32 getipaddr(char *);

/*******************************************************************************
*
*	mkdbdomain
*
*	Recreates the domain database.
*/
void
mkdbdomain(void)
{
	brain(QDOMAINDB, QDOMAIN, &parse);
}

/*******************************************************************************
*
*	parse
*
*	Parses domain file.
*
*	Output:
*		s: 0.
*		f: !0.
*/
static int
parse(FILE *file)
{
	char line[HUGELINELEN+2];
	char *ptr;
	int len;

	lineno=0;
	while (fgets(line, HUGELINELEN, file)) {
		lineno++;
		len=strlen(line);
		if (line[len-1]=='\n')
			line[len-1]='\x00';
		
		ptr=line;
		SKIPBLANKS(ptr);
		if (!*ptr || *ptr=='#')	/* eol or comment? */
			continue;

		parseline(ptr);
	}

	if (!feof(file))
		return 1;

	return 0;
}

/*******************************************************************************
*
*	parseline
*
*	Parse line, adding to domain database if necessary.
*
*	Input:
*		src - where to start from.
*/
static void
parseline(char *src)
{
	char *ssrc=src, *ip;
	int ipcnt=0, err;
	bit32 ipaddrs[IPADDRSPRDOMAIN], ipa;

	SKIPTOWHITE(ssrc);	/* domain in *src */
	*ssrc++='\x00';

	for (;;) {
		SKIPBLANKS(ssrc);

		if (!*ssrc || *ssrc=='#')
			break;

		ip=ssrc;
		for (; (isdigit((int)*ssrc) || *ssrc=='.'); ssrc++);
		if (isspace((int)*ssrc) || !*ssrc)
			*ssrc++='\x00';
		else {
			fprintf(stderr, "invalid ip address (%s) on line %i\n", ip, lineno);
			ssrc++;
			continue;
		}


		ipa=getipaddr(ip);
		if (ipa==-1)
			fprintf(stderr, "invalid ip address (%s) on line %i\n", ip, lineno);
		else {
			if (ipcnt<IPADDRSPRDOMAIN)
				ipaddrs[ipcnt++]=ipa;
			else
				fprintf(stderr, "ip-addresses pr. domain count exhausted\n");
		}
	}

	if (ipcnt) {
		err=adddblistentry(&dbl, src, (char *)&ipaddrs, ipcnt*sizeof(bit32));
		if (err)
			fprintf(stderr, "cannot add entry to database\n");
	}
}

/*******************************************************************************
*
*	getipaddr
*
*	Translates string to ip address.
*
*	Input:
*		src - source string.
*	Output:
*		s: ipv4 address.
*		f: -1.
*/
static bit32
getipaddr(char *src)
{
#ifdef OSLinux
	struct in_addr addr;
	bit32 valid;

	valid=(bit32)inet_aton(src, &addr);
	if (!valid)
		return -1;

	return (bit32)addr.s_addr;
#endif

#ifdef OSOSF1
	int valid;

	valid=inet_addr(src);
	if (valid==(in_addr_t)-1)
		return -1;

	return (bit32)valid;
#endif
}
