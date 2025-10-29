/*
 * StringBuffer.c
 */
#include "Include.h"

/*
 * SBNew
 *
 * Allocate a new stringbuffer and ensure minimum capacity.
 *
 *  -> growth		Growth.
 */
StringBuffer *
SBNew(UInt16 growth)
{
	MemHandle sh=MemHandleNew(sizeof(StringBuffer));
	MemHandle dh=MemHandleNew(growth);
	StringBuffer *sb;

	ErrFatalDisplayIf(dh==NULL || sh==NULL, "(SBNew) Out of memory");
	sb=MemHandleLock(sh);
	sb->capacity=growth;
	sb->growth=growth;
	sb->length=0;
	sb->data=MemHandleLock(dh);

	return sb;
}

/*
 * SBFree
 *
 * Deallocate the string buffer.
 *
 *  -> sb		String buffer.
 */
void
SBFree(StringBuffer *sb)
{
	if (sb) {
		if (sb->data)
			MemPtrFree(sb->data);

		MemPtrFree(sb);
	}
}

/*
 * SBAppend
 *
 * Append a chunk of data to the string buffer.
 *
 *  -> sb		String buffer.
 *  -> src		Source string.
 *  -> len		Source string len.
 */
void
SBAppend(StringBuffer *sb, UInt8 *src, UInt16 len)
{
	while (sb->length+len>sb->capacity) {
		MemHandle mh;
		Char *p;
		UInt32 i;

		i=((UInt32)sb->capacity)+((UInt32)sb->growth);
		ErrFatalDisplayIf(i>65534, "(SBAppend) StringBuffer exploded");
		sb->capacity=(UInt16)i;
		mh=MemHandleNew(sb->capacity);
		ErrFatalDisplayIf(mh==NULL, "(SBAppend) Out of memory");
		p=MemHandleLock(mh);
		MemMove(p, sb->data, sb->length);
		MemPtrFree(sb->data);
		sb->data=p;
	}

	MemMove(sb->data+sb->length, src, len);
	sb->length+=len;
}

/*
 * SBGetData
 *
 * Get start of data and length of data.
 *
 *  -> sb		String buffer.
 * <-  len		Where to store length.
 *
 * Returns data pointer.
 */
UInt8 *
SBGetData(StringBuffer *sb, UInt16 *len)
{
	*len=sb->length;
	return sb->data;
}

/*
 * SBReset
 *
 * Reset pointers = empty stringbuffer.
 *
 *  -> sb		String buffer.
 */
void
SBReset(StringBuffer *sb)
{
	sb->length=0;
}
