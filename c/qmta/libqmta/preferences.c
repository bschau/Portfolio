#include "private.h"

/* globals */
static int lineno;
static struct {
	char *name;
	int value;
} logfac[]={
	{ "LOG_DAEMON", LOG_DAEMON },
	{ "LOG_MAIL", LOG_MAIL },
	{ "LOG_USER", LOG_USER },
	{ "LOG_LOCAL0", LOG_LOCAL0 },
	{ "LOG_LOCAL1", LOG_LOCAL1 },
	{ "LOG_LOCAL2", LOG_LOCAL2 },
	{ "LOG_LOCAL3", LOG_LOCAL3 },
	{ "LOG_LOCAL4", LOG_LOCAL4 },
	{ "LOG_LOCAL5", LOG_LOCAL5 },
	{ "LOG_LOCAL6", LOG_LOCAL6 },
	{ "LOG_LOCAL7", LOG_LOCAL7 },
	{ NULL, 0 },
};

/* protos */
static void dconf(cfa *, int);
static int dconflength(cfa *);
static int parse(cfa *, char *);
static char *findeol(char *);
static char *openconfig(char *);
static void closeconfig(char **);
static void setdefault(cfa *);
static int strtofac(char *);
static char *factostr(int);

/*******************************************************************************
*
*	setroot
*
*	Fixate root path.
*/
void
setroot(void)
{
	qroot=getenv("QROOT");
	if (!qroot)
		qroot=QROOT;
}

/*******************************************************************************
*
*	configure
*
*	Configures instance.
*
*	Output:
*		s: 0.
*		f: !0.
*/
int
configure(void)
{
	char *cnf;
	int err;
	
	cnf=openconfig(QCNF);
	if (!cnf) {
		fprintf(stderr, "cannot open configuration file %s/" QCNF "\n", qroot);
		exit(1);
	}

	err=parse(cfaconfig, cnf);
	if (err)
		return 1;

	setdefault(cfaconfig);

	closeconfig(&cnf);

	logfacility=strtofac(privlogf);
	if (logfacility==-1)
		logfacility=LOG_LOCAL0;

	free(privlogf);
	privlogf=strdup(factostr(logfacility));

	sanity();
	return 0;
}

/*******************************************************************************
*
*	deconfigure
*
*	Deconfigure cfa structure.
*/
void
deconfigure(void)
{
	int cnt;

	for (cnt=0; cfaconfig[cnt].name; cnt++) {
		if (cfaconfig[cnt].type==CFASTR) {
			if (*(cfaconfig[cnt].sdst)) {
				free(*(cfaconfig[cnt].sdst));
				*(cfaconfig[cnt].sdst)=NULL;
			}
		}
	}
}

/*******************************************************************************
*
*	dumpconfig
*
*	Dumps configuration.
*/
void
dumpconfig(void)
{
	int width;

	width=dconflength(cfaconfig);
	printf("Parsed configuration:\n\n");
	dconf(cfaconfig, width);
	printf("\n(*) default\n");
}

/*******************************************************************************
*
*	helpconfig
*
*	Writes help and configuration for switch.
*
*	Input:
*		s - switch.
*	Output:
*		s: 0.
*		f: !0.
*/
int
helpconfig(char *s)
{
	int cnt;

	for (cnt=0; cfaconfig[cnt].name; cnt++) {
		if (strcasecmp(cfaconfig[cnt].name, s))
			continue;

		printf("%s - %s\n\n%s\nCurrently set to: ", cfaconfig[cnt].name, cfaconfig[cnt].description, *(cfaconfig[cnt].help));
		if (cfaconfig[cnt].type==CFAINT)
			printf("%i", *(cfaconfig[cnt].idst));
		else
			printf("%s", *(cfaconfig[cnt].sdst));

		if (cfaconfig[cnt].private==-1)
			printf(" (default)\n");
		else
			printf(" (configured)\n");

		return 0;
	}

	fprintf(stderr, "no help on '%s'\n", s);
	return 1;
}

/*******************************************************************************
*
*	varconfig
*
*	Writes value for switch.
*
*	Input:
*		s - switch.
*	Output:
*		s: 0.
*		f: !0.
*/
int
varconfig(char *s)
{
	int cnt;

	for (cnt=0; cfaconfig[cnt].name; cnt++) {
		if (strcasecmp(cfaconfig[cnt].name, s))
			continue;

		if (cfaconfig[cnt].type==CFAINT)
			printf("%i\n", *(cfaconfig[cnt].idst));
		else
			printf("%s\n", *(cfaconfig[cnt].sdst));

		return 0;
	}

	return 1;
}

/*******************************************************************************
*
*	dconf
*
*	Traverse a cfa structure and format/print values.
*
*	Input:
*		c - cfa structure.
*		width - width.
*/
static void
dconf(cfa *c,
      int width)
{
	int len;

	for (; c->name; c++) {
		printf("%s (%s): ", c->description, c->name);
		len=width-strlen(c->description)-strlen(c->name);
		if (len>0) {
			for (len--; len>0; len--)
				printf(".");
			printf(" ");
		}

		printf("%s", ((c->private!=-1) ? "" : "(*) "));
		switch (c->type) {
			case CFAINT:
				printf("%i\n", *(c->idst));
				break;
			case CFASTR:
				if (*(c->sdst))
					printf("%s\n", *(c->sdst));
				else
					printf("n/a\n");
				break;
		}
	}
}

/*******************************************************************************
*
*	dconflength
*
*	Find max width of descriptions.
*
*	Input:
*		c - cfa structure.
*/
static int
dconflength(cfa *c)
{
	int width=0, len;

	for (; c->name; c++) {
		len=strlen(c->description)+strlen(c->name);
		if (len>width)
			width=len;
	}

	return width;
}

/*******************************************************************************
*
*	parse
*
*	Parses config file.
*
*	Input:
*		c - cfa config.
*		m - in-memory representation of configuration file.
*	Output:
*		s: 0.
*		f: >0 (number of errors encountered).
*/
static int
parse(cfa *c,
      char *m)
{
	int errors=0;
	char *ls, *token, *value;
	cfa *t;

	lineno=0;

	for (EVER) {
		ls=m;
		m=findeol(m);
		if (!m)		/* all done */
			break;

		SKIPBLANKS(ls);
		if (!*ls)	/* all blank line */
			continue;

		token=ls;
		SKIPTOWHITE(ls);
		if (*ls)
			*ls++='\x00';

		/* do we know this token? */
		for (t=c; t->name; t++) {
			if (!(strcasecmp(t->name, token)))
				break;
		}

		if (!t->name) {
			fprintf(stderr, "%i: unknown directive (%s)\n", lineno, token);
			errors++;
			continue;
		}

		/* have we seen it before? */
		if (t->private) {
			fprintf(stderr, "%i: %s already set on line %i\n", lineno, token, t->private);
			errors++;
			continue;
		}

		/* find argument */
		SKIPBLANKS(ls);
		if (!*ls) {
			fprintf(stderr, "%i: missing argument to %s\n", lineno, token);
			errors++;
			continue;
		}

		value=ls;
		SKIPTOWHITE(ls);
		*ls='\x00';
		switch (t->type) {
			case CFAINT:
				*(t->idst)=atoi(value);
				break;
			case CFASTR:
				value=strdup(value);
				if (value)
					*(t->sdst)=value;
				else {
					fprintf(stderr, "out of memory\n");
					exit(1);
				}
				break;
		}

		t->private=lineno;
	}

	return errors;
}

/*******************************************************************************
*
*	findeol
*
*	Finds end of line.
*
*	Input:
*		m - start of line.
*	Output:
*		s: end of line.
*		f: NULL (end of data).
*/
static char *
findeol(char *m)
{
	char *ls;

	if (!*m)
		return NULL;

	ls=m;
	FINDEOL(m);
	lineno++;
	if (*m)
		*m++='\x00';

	/* zap comments */
	for (; *ls && *ls!='#'; ls++);
	if (*ls)	/* found a comment */
		*ls='\x00';

	return m;
}

/*******************************************************************************
*
*	openconfig
*
*	Open and allocates config file.
*
*	Input:
*		cf - config file.
*	Output:
*		s: in-memory representation.
*		f: NULL.
*/
static char *
openconfig(char *cf)
{
	int len, r;
	FILE *f;
	char *mem;

	len=getfilesize(cf);
	if (len<0)
		return NULL;

	f=fopen(cf, "r");
	if (!f)
		return NULL;

	mem=(char *)calloc(len+1, 1);
	if (!mem) {
		fclose(f);
		return NULL;
	}

	r=fread(mem, 1, len, f);
	fclose(f);
	if (r!=len) {
		free(mem);
		mem=NULL;
	}

	return mem;
}

/*******************************************************************************
*
*	closeconfig
*
*	Close/deallocates config file.
*
*	Input:
*		imp - pointer to pointer to in-memory representation.
*/
static void
closeconfig(char **imr)
{
	if (*imr) {
		free(*imr);
		*imr=NULL;
	}
}

/*******************************************************************************
*
*	setdefault
*
*	Set up default switches.
*
*	Input:
*		c - cfa config.
*/
static void
setdefault(cfa *c)
{
	while (c->name) {
		if (!c->private) {
			switch (c->type) {
				case CFAINT:
					*(c->idst)=c->iinit;
					break;
				case CFASTR:
					if (c->sinit)
						*(c->sdst)=strdup(c->sinit);
					else
						*(c->sdst)=NULL;
					break;
			}
			c->private=-1;
		}

		c++;
	}
}

/*******************************************************************************
*
*	strtofac
*
*	Translates a LOG_? string to a LOG_? constant.
*
*	Input:
*		sf - string facility.
*	Output:
*		s: facility.
*		f: -1.
*/
static int
strtofac(char *sf)
{
	int cnt;

	if (!sf)
		return -1;

	for (cnt=0; logfac[cnt].name; cnt++) {
		if (!strcasecmp(logfac[cnt].name, sf))
			return logfac[cnt].value;
	}

	return -1;
}

/*******************************************************************************
*
*	factostr
*
*	Returns pointer to facility name.
*
*	Input:
*		f - facility.
*	Output:
*		s: facility name.
*		f: NULL.
*/
static char *
factostr(int f)
{
	int cnt;

	if (f==-1)
		return NULL;

	for (cnt=0; logfac[cnt].name; cnt++) {
		if (logfac[cnt].value==f)
			return logfac[cnt].name;
	}

	return NULL;
}
