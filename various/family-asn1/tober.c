#include "family.h"

/* macros */
#define LINELENGTH 1024
#define DATASIZE 1024

/* protos */
static int Die(char *, int);
static char *Tokenize(char **);
static void PickUpString(AsnOcts *, char **);

/* globals */
static FILE *iFile, *oFile;

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

	if (oFile)
		fclose(oFile);

	if (iFile)
		fclose(iFile);

	exit(eC);
}

/*
**	Main
*/
int
main(int argc, char *argv[])
{
	int dSize=DATASIZE;
	char line[LINELENGTH+1], dArray[DATASIZE];
	char *p, *s;
	SBuf outBuf;
	FamilyRecord v;
	long int len;
	Person **kids;

	InitNibbleMem(512, 512);

	if ((iFile=fopen("family.txt", "r"))==NULL)
		Die("Cannot open family.txt file.", 1);

	if ((oFile=fopen(FAMILYBER, "w+"))==NULL)
		Die("Cannot create " FAMILYBER " file.", 1);

	while ((p=fgets(line, LINELENGTH-1, iFile))) {
		for (s=p; *s; s++);
		s--;
		while (s>=p) {
			if (*s=='\r' || *s=='\n')
				*s='\x00';
			else
				break;

			s--;
		}

		if (*p) {
			if (*p=='#')
				continue;

			memset(&v, 0, sizeof(FamilyRecord));
			PickUpString(&v.familyName, &p);

			v.father=Asn1Alloc(sizeof(Person));
			v.father->name=Asn1Alloc(sizeof(Name));
			PickUpString(&v.father->name->firstName, &p);
			PickUpString(&v.father->name->initial, &p);
			PickUpString(&v.father->dateOfBirth, &p);

			v.mother=Asn1Alloc(sizeof(Person));
			v.mother->name=Asn1Alloc(sizeof(Name));
			PickUpString(&v.mother->name->firstName, &p);
			PickUpString(&v.mother->name->initial, &p);
			PickUpString(&v.mother->dateOfBirth, &p);

			v.kids=AsnListNew(sizeof(void *));

			while (*p) {
				kids=AsnListAppend(v.kids);
				*kids=Asn1Alloc(sizeof(Person));
				(*kids)->name=Asn1Alloc(sizeof(Name));
				PickUpString(&(*kids)->name->firstName, &p);
				PickUpString(&(*kids)->name->initial, &p);
				(*kids)->dateOfBirth.octs=Asn1Alloc(sizeof(Date));
				PickUpString(&(*kids)->dateOfBirth, &p);
			}

			SBufInit(&outBuf, dArray, dSize);
			SBufResetInWriteRvsMode(&outBuf);

			len=BEncFamilyRecord(&outBuf, &v);
			if ((len<=0) || (SBufWriteError(&outBuf)))
				Die("Cannot encode FamilyRecord.", 1);

			SBufResetInReadMode(&outBuf);
			for (; len>0; len--) {
				if (SBufEod(&outBuf))
					Die("Sudden end in buffer.", 1);

				fputc(SBufGetByte(&outBuf), oFile);
			}

			ResetNibbleMem();
		}
	}

	Die(NULL, 0);
	exit(0);	// GCC, please shut up!
}

/*
**	Tokenize
**
**	Get next token.
**
**	<-> t		Token string.
**
**	Returns next token (or NULL), advances pointer.
*/
static char *
Tokenize(char **t)
{
	char *token=*t, *s=*t;

	if (!*s)
		return NULL;

	for (; *s && *s!=','; s++);
	if (*s)
		*s++='\x00';
	*t=s;

	return token;
}

/*
**	PickUpString
**
**	Get next string.
**
**	 -> d		Where to store string.
**	<-> p		Source string.
*/
static void
PickUpString(AsnOcts *d, char **p)
{
	if ((d->octs=Tokenize(p))==NULL)
		d->octetLen=0;
	else
		d->octetLen=strlen(d->octs);
}
