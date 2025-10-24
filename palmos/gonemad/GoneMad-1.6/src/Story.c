/*
 * Story.c
 */
#include <DLServer.h>
#include "Include.h"

/* macros */
#define KeywordLen 100

/* structs */
typedef struct {
	UInt16 len;
	Char keyword[KeywordLen+1];
	Char value[ValueLen+1];
} Keyword;

typedef struct {
	UInt32 uid;
} StoryHdr;

/* protos */
static Boolean KeywordExists(Char *, UInt16, UInt16 *);
static void Copy(MemHandle, UInt32 *, Char *, UInt16);
static UInt16 FindKeywordBlock(UInt32);

/* globals */
static MemHandle storyH, keywordH;
static Char *story;
static Keyword *keyword;
static UInt16 storyIdx, storyCnt;

/*
 * KeywordExists
 *
 * Do a keyword lookup.
 *
 *  -> src		Source keyword.
 *  -> len		Keyword length.
 * <-  idx		Index found (or NULL).
 *
 * Returns true if found, false otherwise.
 */
static Boolean
KeywordExists(Char *src, UInt16 len, UInt16 *idx)
{
	UInt16 i;

	for (i=0; i<storyCnt; i++) {
		if (keyword[i].len==len) {
			if (StrNCaselessCompare(keyword[i].keyword, src, len)==0) {
				if (idx)
					*idx=i;

				return true;
			}
		}
	}

	return false;	
}

/*
 * StoryDone
 *
 * Deallocate the story.
 */
void
StoryDone(void)
{
	if (keywordH) {
		MemHandleFree(keywordH);
		keywordH=NULL;
	}

	if (storyH) {
		MemHandleFree(storyH);
		storyH=NULL;
	}
}

/*
 * StoryGet
 *
 * Retrieve and parse the story given by the record index.
 *
 *  -> rIdx		Record Index.
 */
void
StoryGet(UInt16 rIdx)
{
	MemHandle mh=DBGetRecord(&dbTemplate, rIdx);
	Boolean inBracket=false;
	UInt32 size, uid;
	UInt16 len, idx;
	Char *p, *s;

	StoryDone();
	storyIdx=rIdx;

	size=MemHandleSize(mh);
	storyH=MemHandleNew(size+2);
	ErrFatalDisplayIf(storyH==NULL, "(StoryGet) Out of memory.");
	story=MemHandleLock(storyH);

	MemMove(story, MemHandleLock(mh), size);
	MemHandleUnlock(mh);

	story[size]='\x00';
	story[size+1]='\x00';

	/* Count number of keywords */
	len=0;
	for (p=story; *p; p++) {
		if (*p=='[') {
			if (*(p+1)!='[' && *(p+1)!=']' && *(p+1)!='*') {
				if (inBracket==false)
					inBracket=true;
			}
		} else if (*p==']') {
			if (inBracket==true) {
				len++;
				inBracket=false;
			}
		}
	}

	if (len>MaxKeywords)
		len=MaxKeywords;

	keywordH=MemHandleNew(sizeof(Keyword)*(len+1));
	ErrFatalDisplayIf(keywordH==NULL, "(StoryGet) Out of memory.");
	keyword=MemHandleLock(keywordH);

	for (size=0; size<(len+1); size++)
		MemSet(&keyword[size].len, sizeof(Keyword), 0);

	/* Parse .. */
	inBracket=false;
	size=0;
	s=NULL;
	len=0;
	storyCnt=0;
	for (p=story; *p; p++) {
		if (*p=='[') {
			if (*(p+1)!='[' && *(p+1)!=']' && *(p+1)!='*') {
				if (inBracket==false) {
					len=0;
					s=p+1;
					inBracket=true;
				}
			}
		} else if (*p==']') {
			if (inBracket) {
				if (KeywordExists(s, len, NULL)==false) {
					if (len>KeywordLen)
						len=KeywordLen;

					keyword[storyCnt].len=len;
					MemMove(keyword[storyCnt].keyword, s, len);
					keyword[storyCnt].keyword[len]='\x00';

					storyCnt++;
					if (storyCnt>=MaxKeywords)
						break;
				}
				inBracket=false;
			}
		} else if (inBracket)
			len++;
	}

	/* load keywords */
	DmRecordInfo(dbTemplate.db, storyIdx, NULL, &uid, NULL);
	if ((rIdx=FindKeywordBlock(uid))==dmMaxRecordIndex)
		return;

	mh=DBGetRecord(&dbKeyword, rIdx);
	p=MemHandleLock(mh);
	p+=sizeof(StoryHdr);
	while (*p) {
		len=(UInt16)(*p++);
		if (KeywordExists(p, len, &idx)==true) {
			p+=len;
			len=(UInt16)(*p++);
			MemMove(keyword[idx].value, p, len);
			keyword[idx].value[len]='\x00';
		} else {
			p+=len;
			len=(UInt16)(*p++);
		}
		p+=len;
	}

	MemHandleUnlock(mh);
}

/*
 * StoryGetCount
 *
 * Get number of keywords in story.
 */
UInt16
StoryGetCount(void)
{
	return storyCnt;
}

/*
 * StoryGetKeyword
 *
 * Get the keyword at position X.
 *
 *  -> pos		Position.
 *
 * Returns string ptr or NULL (if not found).
 */
Char *
StoryGetKeyword(UInt16 pos)
{
	if (pos<storyCnt)
		return keyword[pos].keyword;

	return NULL;
}

/*
 * StoryGetKeywordLength
 *
 * Get the length of the keyword at position X.
 *
 *  -> pos		Position.
 *
 * Returns length.
 */
UInt16
StoryGetKeywordLength(UInt16 pos)
{
	if (pos<storyCnt)
		return keyword[pos].len;

	return 0;
}

/*
 * StoryGetValue
 *
 * Get the value for keyword at position X.
 *
 *  -> pos		Position.
 *
 * Returns string ptr or NULL (if not found).
 */
Char *
StoryGetValue(UInt16 pos)
{
	if (pos<storyCnt && StrLen(keyword[pos].value)>0)
		return keyword[pos].value;

	return NULL;
}

/*
 * StorySetValue
 *
 * Set the value for keyword X.
 *
 *  -> pos		Position
 *  -> value		Value.
 */
void
StorySetValue(UInt16 pos, Char *value)
{
	if (pos<storyCnt) {
		if (value) {
			UInt16 len=StrLen(value);

			if (len>ValueLen)
				len=ValueLen;

			if (len>0)
				MemMove(keyword[pos].value, value, len);
	
			keyword[pos].value[len]='\x00';
		} else
			keyword[pos].value[0]='\x00';
	}
}

/*
 * StoryContainsBlanks
 *
 * See if all keywords are valid.
 *
 * Returns true if a blank if found, false otherwise.
 */
Boolean
StoryContainsBlanks(void)
{
	UInt16 idx;

	for (idx=0; idx<storyCnt; idx++) {
		if (!*keyword[idx].value)
			return true;
	}

	return false;
}

/*
 * StoryGetFinal
 *
 * Create the final story.
 *
 * Return MemHandle.
 */
MemHandle
StoryGetFinal(void)
{
	Char bufName[dlkUserNameBufSize+1], bufDate[longDateStrLength+1], bufTime[timeStringLength+1];
	Char *p=story, *s=story;
	UInt32 cCnt=0;
	DateTimeType dt;
	MemHandle mh;

	mh=MemHandleNew(MemoSize);
	ErrFatalDisplayIf(mh==NULL, "(StoryGetFinal) Out of memory.");

	if (DlkGetSyncInfo(NULL, NULL, NULL, bufName, NULL, NULL)!=errNone)
		StrCopy(bufName, "Eddie the Psyched PDA");

	TimSecondsToDateTime(TimGetSeconds(), &dt);
	DateToAscii(dt.month, dt.day, dt.year, PrefGetPreference(prefLongDateFormat), bufDate);
	TimeToAscii(dt.hour, dt.minute, PrefGetPreference(prefTimeFormat), bufTime);

	for (; *p; p++) {
		if (*p=='[') {
			if (*(p+1)=='[' || *(p+1)==']')
				continue;

			Copy(mh, &cCnt, s, p-s); 

			for (s=p+1; *s; s++) {
				if (*s==']') {
					if (*(p+1)=='*') {
						UInt16 len=s-p-2;

						if (len==6 && StrNCaselessCompare(p+2, "advice", len)==0)
							Copy(mh, &cCnt, "Don't get lost!", 15);
						else if (len==19 && StrNCaselessCompare(p+2, "the meaning of life", len)==0)
							Copy(mh, &cCnt, "42", 2);
						else if (len==4 && StrNCaselessCompare(p+2, "date", len)==0)
							Copy(mh, &cCnt, bufDate, StrLen(bufDate));
						else if (len==4 && StrNCaselessCompare(p+2, "time", len)==0)
							Copy(mh, &cCnt, bufTime, StrLen(bufTime));
						else if (len==4 && StrNCaselessCompare(p+2, "name", len)==0)
							Copy(mh, &cCnt, bufName, StrLen(bufName));
					} else {
						UInt16 len=s-p-1, i;

						for (i=0; i<storyCnt; i++) {
							if (keyword[i].len==len) {
								if (StrNCaselessCompare(keyword[i].keyword, p+1, len)==0) {
									Copy(mh, &cCnt, keyword[i].value, StrLen(keyword[i].value));
									break;
								}
							}
						}
					}

					s++;
					p=s;
					break;
				}
			}

			if (!*s) {
				Copy(mh, &cCnt, p, s-p);
				return mh;
			}
		}
	}

	if (s<p)
		Copy(mh, &cCnt, s, p-s);

	return mh;
}

/*
 * Copy
 *
 * Copy text to memhandle.
 *
 *  -> mh		MemHandle.
 * <-> cCnt		# of character
 *  -> src		Source.
 *  -> len		Length.
 */
static void
Copy(MemHandle mh, UInt32 *cCnt, Char *src, UInt16 len)
{
	UInt32 c=*cCnt;
	Char *p;

	if ((len+1)>c) {
		ErrFatalDisplayIf(MemHandleResize(mh, MemHandleSize(mh)+MemoSize)!=errNone, "(Copy) Out of memory.");
	}

	p=MemHandleLock(mh);
	MemMove(p+c, src, len);
	c+=len;
	p[c]='\x00';
	MemHandleUnlock(mh);
	*cCnt=c;
}

/*
 * FindKeywordBlock
 *
 * Find keyword block in database.
 *
 *  -> uid		Story unique id.
 *
 * Return keyword block index if found, dmMaxRecordIndex if not found.
 */
static UInt16
FindKeywordBlock(UInt32 uid)
{
	DmOpenRef db=dbKeyword.db;
	UInt16 idx=0;
	MemHandle mh;

	for (EVER) {
		if ((mh=DmQueryNextInCategory(db, &idx, dmAllCategories))==NULL)
			break;

		if (((StoryHdr *)MemHandleLock(mh))->uid==uid) {
			MemHandleUnlock(mh);
			return idx;
		}
		MemHandleUnlock(mh);
		idx++;
	}

	return dmMaxRecordIndex;
}

/*
 * StorySaveKeywords
 *
 * Save keywords to database.
 *
 * Returns true if keywords was saved, false otherwise.
 */
Boolean
StorySaveKeywords(void)
{
	DmOpenRef db=dbKeyword.db;
	UInt32 size=0, uid;
       	UInt16 rIdx, idx, len, slot, attrs;
	UInt8 b;
	MemHandle mh;
	void *p;

	for (idx=0; idx<storyCnt; idx++) {
		if ((len=StrLen(keyword[idx].value))>0)
			size+=1+keyword[idx].len+1+len;
	}

	DmRecordInfo(dbTemplate.db, storyIdx, NULL, &uid, NULL);
	rIdx=FindKeywordBlock(uid);

	if (!size) {
		if (rIdx!=dmMaxRecordIndex)
			DmDeleteRecord(db, rIdx);

		return false;
	}

	size+=sizeof(StoryHdr)+1;

	if (rIdx!=dmMaxRecordIndex)
		mh=DmResizeRecord(db, rIdx, size);
	else
		mh=DmNewRecord(db, &rIdx, size);

	ErrFatalDisplayIf(mh==NULL, "(StorySaveKeywords) Out of memory.");

	p=MemHandleLock(mh);
	DmWrite(p, 0, &uid, sizeof(UInt32));
	idx=sizeof(UInt32);
	for (slot=0; slot<storyCnt; slot++) {
		if ((len=StrLen(keyword[slot].value))>0) {
			b=(UInt8)keyword[slot].len;
			DmWrite(p, idx, &b, 1);
			idx++;
			DmWrite(p, idx, keyword[slot].keyword, keyword[slot].len);
			idx+=keyword[slot].len;

			b=(UInt8)len;
			DmWrite(p, idx, &b, 1);
			idx++;
			DmWrite(p, idx, keyword[slot].value, len);
			idx+=len;
		}
	}	

	b=0;
	DmWrite(p, idx, &b, 1);

	MemHandleUnlock(mh);

	DmRecordInfo(db, rIdx, &attrs, NULL, NULL);
	attrs&=~dmRecAttrCategoryMask;
	attrs|=dmUnfiledCategory;
	DmSetRecordInfo(db, rIdx, &attrs, NULL);
	DmReleaseRecord(db, rIdx, true);
	return true;
}

/*
 * StoryClearKeywords
 *
 * Clear all keywords.
 */
void
StoryClearKeywords(void)
{
	UInt32 uid;
	UInt16 idx;

	for (idx=0; idx<storyCnt; idx++)
		MemSet(keyword[idx].value, ValueLen+1, 0);

	DmRecordInfo(dbTemplate.db, storyIdx, NULL, &uid, NULL);
	if ((idx=FindKeywordBlock(uid))==dmMaxRecordIndex)
		return;

	DmDeleteRecord(dbKeyword.db, idx);
}
