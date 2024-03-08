#include "family.h"

/* macros */

/* protos */
static int Die(char *, int);
static void LoadBer(void);
static void DumpPerson(Person *, char *);
static void DumpFamily(FamilyRecord *);
static char *StrToLower(char *);
static int Find(char *, char *);
static int FindInPerson(Person *, char *);
static void DumpBer(void);
static void SearchBer(char *);
static void FamilyBer(char *);

/* globals */
static FILE *iFile;
static char *ber;
static int berSize;
static SBuf sBuf;

/*
**	Die
**
**	Deallocates everything and exits.
**
**	 -> eT		Error text.
**	 -> eC		Error code.
*/
static int
Die(char *eT, int eC)
{
	if (eT)
		fprintf(stderr, "%s\n", eT);

	if (ber)
		free(ber);

	if (iFile)
		fclose(iFile);

	exit(eC);
}

/*
**	LoadBer
**
**	Load ber file.
*/
static void
LoadBer(void)
{
	if ((iFile=fopen(FAMILYBER, "r"))==NULL)
		Die("Cannot open " FAMILYBER " file.", 1);

	fseek(iFile, 0, SEEK_END);
	berSize=ftell(iFile);
	rewind(iFile);

	if ((ber=calloc(berSize+1, 1))==NULL)
		Die("Out of memory.", 1);

	if (fread(ber, berSize, 1, iFile)!=1)
		Die("Cannot load " FAMILYBER " file.", 1);
}

/*
**	DumpPerson
**
**	Dump a person record.
**
**	 -> p		Person.
**	 -> s		Prefix.
*/
static void
DumpPerson(Person *p, char *s)
{
	int canFirst=0, canInitial=0, canBorn=0;

	if (p->name->firstName.octs && p->name->firstName.octetLen>0)
		canFirst=1;

	if (p->name->initial.octs && p->name->initial.octetLen>0)
		canInitial=1;

	if (p->dateOfBirth.octs && p->dateOfBirth.octetLen>0)
		canBorn=1;

	if (canFirst || canInitial || canBorn) {
		if (s)
			printf("%s", s);
	}

	if (canFirst==0 && canInitial==0)
		printf("No name");
	else {
		if (canFirst)
			printf("%s", p->name->firstName.octs);

		if (canInitial) {
			if (canFirst)
				printf(" ");

			printf("%s", p->name->initial.octs);
		}
	}

	if (canBorn)
		printf(", born: %s", p->dateOfBirth.octs);
}

/*
**	DumpFamily
**
**	Dump family record.
**
**	 -> v		Family Record.
*/
static void
DumpFamily(FamilyRecord *v)
{
	char e;
	AsnListNode *n;
	int c;

	if (v->familyName.octs && v->familyName.octetLen>0) {
		e=v->familyName.octs[v->familyName.octetLen-1];
		if (e=='s' || e=='S')
			printf("Meet the %s'\n", v->familyName.octs);
		else
			printf("Meet the %ss\n", v->familyName.octs);
	} else
		printf("Unknown family name\n");

	DumpPerson(v->father, "  Father: ");
	printf("\n");

	DumpPerson(v->mother, "  Mother: ");
	printf("\n");

	if (v->kids && v->kids->count>0) {
		if (v->kids->count==1)
			printf("  kid:\n");
		else
			printf("  kids:\n");

		n=v->kids->first;
		for (c=0; c<v->kids->count; c++) {
			DumpPerson((Person *)(n->data), "    ");
			printf("\n");
			n=n->next;
		}
	}
}

/*
**	StrToLower
**
**	Convert source string to lowercase.
**
**	 -> s		Source string.
**
**	Returns string in lowercase.
*/
static char *
StrToLower(char *s)
{
	char *l=strdup(s), *p;

	if (l==NULL)
		Die("Cannot duplicate string.", 1);

	for (p=l; *p; p++)
		*p=(char)tolower((int)*p);

	return l;
}

/*
**	Find
**
**	Find needle in haystack. Needle must be in lowercase.
**
**	 -> haystack	To search in.
**	 -> needle	To search for.
**
**	Return 0 if found, !0 otherwise.
*/
static int
Find(char *haystack, char *needle)
{
	char *h=StrToLower(haystack);
	int s=0;

	if (strstr(h, needle)==NULL)
		s=1;

	free(h);
	return s;
}

/*
**	FindInPerson
**
**	Locate substring in a person record.
**
**	 -> p		Person.
**	 -> needle	Needle to search for.
**
**	Returns 0 if found, !0 otherwise.
*/
static int
FindInPerson(Person *p, char *needle)
{
	if (p->name->firstName.octs && p->name->firstName.octetLen>0) {
		if (Find(p->name->firstName.octs, needle)==0)
			return 0;
	}

	if (p->name->initial.octs && p->name->initial.octetLen>0) {
		if (Find(p->name->initial.octs, needle)==0)
			return 0;
	}

	if (p->dateOfBirth.octs && p->dateOfBirth.octetLen>0) {
		if (Find(p->dateOfBirth.octs, needle)==0)
			return 0;
	}

	return 1;
}

/*
**	Main
*/
int
main(int argc, char *argv[])
{
	if (argc<2)
		Die("Usage: fq [-d | -s search ] familyname", 1);

	LoadBer();
	InitNibbleMem(512, 512);
	SBufInstallData(&sBuf, ber, berSize);

	if (strcmp(argv[1], "-d")==0)
		DumpBer();
	else if (strcmp(argv[1], "-s")==0) {
		if (argc==3)
			SearchBer(argv[2]);
		else
			Die("You must supply a search string.", 1);
	} else
		FamilyBer(argv[1]);

	exit(0);
}

/*
**	DumpBer
**
**	Dump the content of the ber file to stdout.
*/
static void
DumpBer(void)
{
	int decodedSoFar=0, val;
	AsnLen dLen;
	FamilyRecord v;
	jmp_buf env;

	while (decodedSoFar<berSize) {
		dLen=0;
		if ((val=setjmp(env))==0)
			BDecFamilyRecord(&sBuf, &v, &dLen, env);
		else
			Die("Decode routine failed.", val);

		DumpFamily(&v);
		decodedSoFar+=dLen;
	}
}

/*
**	SearchBer
**
**	Search for the given string in the ber.
**
**	 -> needle	Needle to search for.
*/
static void
SearchBer(char *needle)
{
	int decodedSoFar=0, val, found, c;
	char *f=StrToLower(needle);
	AsnLen dLen;
	FamilyRecord v;
	jmp_buf env;
	AsnListNode *n;

	while (decodedSoFar<berSize) {
		dLen=0;
		if ((val=setjmp(env))==0)
			BDecFamilyRecord(&sBuf, &v, &dLen, env);
		else
			Die("Decode routine failed.", val);

		decodedSoFar+=dLen;
		if (v.familyName.octs && v.familyName.octetLen>0) {
			if (Find(v.familyName.octs, f)==0) {
				DumpFamily(&v);
				continue;
			}
		}

		if (FindInPerson(v.father, f)==0) {
			DumpFamily(&v);
			continue;
		}

		if (FindInPerson(v.mother, f)==0) {
			DumpFamily(&v);
			continue;
		}

		if (v.kids && v.kids->count>0) {
			n=v.kids->first;

			found=0;
			for (c=0; c<v.kids->count; c++) {
				if (FindInPerson((Person *)(n->data), f)==0) {
					found=1;
					break;
				}
				n=n->next;
			}

			if (found==1)
				DumpFamily(&v);
		}
	}
}

/*
**	FamilyBer
**
**	Dump the content of the family to stdout.
**
**	 -> family	Family.
*/
static void
FamilyBer(char *family)
{
	int decodedSoFar=0, val;
	char *f=StrToLower(family);
	AsnLen dLen;
	FamilyRecord v;
	jmp_buf env;

	while (decodedSoFar<berSize) {
		dLen=0;
		if ((val=setjmp(env))==0)
			BDecFamilyRecord(&sBuf, &v, &dLen, env);
		else
			Die("Decode routine failed.", val);

		if (v.familyName.octs && v.familyName.octetLen>0) {
			if (Find(v.familyName.octs, f)==0)
				DumpFamily(&v);
		}

		decodedSoFar+=dLen;
	}
}
