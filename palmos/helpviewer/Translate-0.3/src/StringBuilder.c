/**
 * StringBuilder.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Include.h"

/* structs */
typedef struct {
	unsigned char *data;
	int grow;
	int len;
	int max;
} SB;

/* protos */
static void GrowSB(SB *, int);

/**
 * Allocate a new StringBuilder.
 *
 * @param g Default size (and growth).
 * @return StringBuilder.
 */
StringBuilder
SBNew(int g)
{
	SB *sb=(SB *)calloc(1, sizeof(SB));

	if (!sb) {
		fprintf(stderr, "OOM while allocating StringBuilder");
		exit(1);
	}

	if ((sb->data=(char *)malloc(g))==NULL) {
		fprintf(stderr, "OOM while allocating StringBuilder");
		exit(1);
	}

	sb->max=g;
	sb->grow=g;
	return (StringBuilder)sb;
}

/**
 * Destroy a StringBuilder.
 *
 * @param s StringBuilder.
 */
void
SBFree(StringBuilder s)
{
	SB *sb=(SB *)s;

	if (sb) {
		if (sb->data)
			free(sb->data);

		free(sb);
	}
}

/**
 * Attempt to grow the StringBuilder with at least 'c' characters.
 *
 * @param sb StringBuilder.
 * @param c Min. characters to grow.
 */
static void
GrowSB(SB *sb, int c)
{
	int new=(((sb->len+c)/sb->grow)+1)*sb->grow;
	char *d;

	if ((d=malloc(new))==NULL) {
		fprintf(stderr, "OOM while growing StringBuilder");
		exit(1);
	}

	if (sb->len)
		memmove(d, sb->data, sb->len);

	free(sb->data);
	sb->data=d;
	sb->max=new;
	d[sb->len]=0;
}

/**
 * Append a string to the StringBuilder.
 *
 * @param s StringBuilder.
 * @param src Source string.
 * @param len Length of source string.
 */
void
SBAppend(StringBuilder s, unsigned char *src, int len)
{
	if (len) {
		SB *sb=(SB *)s;

		if (sb->len+len+1>sb->max)
			GrowSB(sb, len+1);

		memmove(sb->data+sb->len, src, len);
		sb->len+=len;
	}
}

/**
 * Append a character to the StringBuilder.
 *
 * @param s StringBuilder.
 * @param src Source character.
 */
void
SBAppendChar(StringBuilder s, unsigned char src)
{
	SB *sb=(SB *)s;

	if (sb->len+2>sb->max)
		GrowSB(sb, 2);

	sb->data[sb->len]=src;
	sb->len++;
}

/**
 * Return the string stored in this StringBuilder.
 *
 * @param s StringBuilder.
 * @return String.
 */
unsigned char *
SBToString(StringBuilder s)
{
	SB *sb=(SB *)s;

	sb->data[sb->len]=0;
	return sb->data;
}

/**
 * Return the length of the string in the StringBuilder.
 *
 * @param s StringBuilder.
 * @return String length.
 */
int
SBLength(StringBuilder s)
{
	return ((SB *)s)->len;
}

/**
 * Set length of data in the StringBuilder.
 *
 * @param s StringBuilder.
 * @param len New length.
 */
void
SBSetLength(StringBuilder s, int len)
{
	SB *sb=(SB *)s;

	if (len>sb->len) {
		fprintf(stderr, "Cannot set length to more than real data length in StringBuilder");
		exit(1);
	}

	sb->len=len;
	sb->data[len]=0;
}

/**
 * Reset the StringBuilder.
 *
 * @param s StringBuilder.
 */
void
SBReset(StringBuilder s)
{
	SB *sb=(SB *)s;

	sb->len=0;
	*sb->data=0;
}

/**
 * Copy content from one SB to another.
 *
 * @param src Source SB.
 * @param dst Destination SB.
 */
void
SBCopyTo(StringBuilder src, StringBuilder dst)
{
	SB *sSrc=(SB *)src, *sDst=(SB *)dst;

	SBReset(dst);
	SBAppend(sDst, SBToString(sSrc), SBLength(sSrc));
}

/**
 * Return position in SB.
 *
 * @param s StringBuilder.
 * @return Current position.
 */
int
SBPosition(StringBuilder s)
{
	SB *sb=(SB *)s;

	return sb->len;
}
