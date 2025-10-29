/*
 * LowScores.c
 */
#include "Include.h"

/* globals */
static LowScoreTable low;

/*
 * LowLoadTable
 *
 * Load high score table.
 */
void
LowLoadTable(void)
{
	if (UtilsGetPref(&low, sizeof(LowScoreTable), PrfLowScores)==false)
		LowReset();
}

/*
 * LowSaveTable
 *
 * Save high score table.
 */
void
LowSaveTable(void)
{
	UtilsSetPref(&low, sizeof(LowScoreTable), PrfLowScores);
}

/*
 * LowGetPair
 *
 * Get name/score at position.
 *
 *  -> pos	Position.
 * <-  name	Destination.
 * <-  dLen	Destination length.
 * <-  score	Score.
 */
void
LowGetPair(UInt16 pos, Char **name, UInt16 *dLen, UInt16 *score)
{
	ErrFatalDisplayIf(pos>5, "(LowGetPair) Out of bounds.");

	pos--;
		
	*score=low.scores[pos];
	*name=&low.names[pos][0];
	*dLen=StrLen(low.names[pos]);
}

/*
 * LowAddPair
 *
 * Add score/name to hiscore table.
 *
 *  -> s	Score.
 *  -> n	Name.
 */
void
LowAddPair(UInt16 s, Char *n)
{
	Int16 pos=(Int16)LowQualify(s);

	if (pos) {
       		Int16 ptr;

		pos--;
		if (pos<4) {
			for (ptr=3; ptr>=pos; ptr--) {
				low.scores[ptr+1]=low.scores[ptr];
				MemMove(&low.names[ptr+1][0], &low.names[ptr][0], dlkUserNameBufSize);
			}
		}

		low.scores[pos]=s;
		MemSet(&low.names[pos][0], dlkUserNameBufSize, 0);
		StrNCopy(&low.names[pos][0], n, dlkUserNameBufSize-1);
	}
}		

/*
 * LowQualify
 *
 * Does this score qualify for a position in the table?
 *
 *  -> s	Score.
 *
 * Return position or 0 (not qualified).
 */
UInt16
LowQualify(UInt16 s)
{
	UInt16 i;

	for (i=0; i<5; i++) {
		if (s<=low.scores[i])
			return i+1;
	}

	return 0;
}

/*
 * LowReset
 *
 * Reset the low scorers list.
 */
void
LowReset(void)
{
	low.scores[0]=9999;
	low.scores[1]=9999;
	low.scores[2]=9999;
	low.scores[3]=9999;
	low.scores[4]=9999;
	StrCopy(low.names[0], "Nobody");
	StrCopy(low.names[1], "Nobody");
	StrCopy(low.names[2], "Nobody");
	StrCopy(low.names[3], "Nobody");
	StrCopy(low.names[4], "Nobody");
}
