/*
 * BMH.c
 */
#include "Include.h"

#define MaxChars 256

typedef struct {
	UInt16 skip[MaxChars];
	UInt16 needleLen;
} BMH;

#if DEBUG
void
BMHTest(void)
{
	Char *needle="tod";
	Char *haystack="Todays agenda: todo list revisition\nTomorrows agenda: todays agenda.";
	void *b;
	Int32 i;
	Char *hP;

	b=BMHOpen(needle);
	ErrFatalDisplayIf(b==NULL, "(Donky) Out of memory.");

	hP=haystack;
	while ((i=BMHSearch(b, hP))>-1) {
		hP+=i;
		hP++;
	}

	BMHClose(b);
}
#endif

/*
 * BMHOpen
 *
 * Setup initial Boyer Moore Horspool.
 */
void *
BMHOpen(Char *needle)
{
	UInt32 len=sizeof(BMH)+StrLen(needle)+1;
	UInt16 nL=0;
	MemHandle mh;
	BMH *b;
	Char *p, *n;
	UInt16 *sP;

	mh=MemHandleNew(len);
	ErrFatalDisplayIf(mh==NULL, "(BMHOpen) Out of memory.");

	b=MemHandleLock(mh);
	p=(UInt8 *)b+sizeof(BMH);
	n=p;

	while (*needle) {
		if (*needle>='a' && *needle<='z')
			*p=*needle-('a'-'A');
		else
			*p=*needle;

		p++;
		needle++;
		nL++;
	}

	*p='\x00';
	b->needleLen=nL;

	sP=&b->skip[0];
	for (len=0; len<MaxChars; len++)
		sP[len]=nL;

	for (len=0; len<nL-1; len++)
		sP[(UInt16)n[len]]=nL-len-1;

	return b;
}

/*
 * BMHSearch
 *
 * Search a string for the occurrence of needle (in BMH).
 *
 *  -> bmh		BMH.
 *  -> source	Source string to search.
 *
 * Returns position in source if found, -1 otherwise.
 */
Int32
BMHSearch(void *bmh, Char *source)
{
	BMH *b=bmh;
	UInt16 sL=StrLen(source), nL;
	Int32 k, j, i;
	Char *n;
	UInt16 *skip;
	Char lotte;

	nL=b->needleLen;
	if (nL>sL)
		return -1;

	n=(UInt8 *)b+sizeof(BMH);
	skip=&b->skip[0];

	k=nL-1;
	while (k<sL) {
		j=nL-1;
		i=k;
		while (j>=0) {
			if (source[i]>='a' && source[i]<='z')
				lotte=source[i]-('a'-'A');
			else
				lotte=source[i];

			if (lotte==n[j]) {
				i--;
				j--;
			} else
				break;
		}

		if (j==-1)
			return i+1;

		if (source[k]>='a' && source[k]<='z')
			lotte=source[k]-('a'-'A');
		else
			lotte=source[k];

		k+=skip[(UInt16)lotte];
	}

	return -1;
}

/*
 * BMHClose
 *
 * Close a BMH query.
 *
 *  -> bmh		BMH.
 */
void
BMHClose(void *bmh)
{
	BMH *b=bmh;

	if (b)
		MemPtrFree(b);
}
