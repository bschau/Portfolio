/*
 * Doc.c
 */
#include "Include.h"

/* protos */
static Boolean BuildIndex(Document *);
static UInt16 Validate(UInt8 *, UInt32);
static void Decompress(UInt8 *, UInt32, UInt8 *, UInt32);

/*
 * DocOpen
 *
 * Open the specified document.
 *
 *  -> name		Document to open.
 *  -> path		Path to document (or NULL if in memory).
 *  -> pos		Start position.
 */
Document *
DocOpen(Char *name, Char *path, UInt32 pos)
{
	MemHandle mh=MemHandleNew(sizeof(Document)), rh;
	Document *d;
	DOCHeader *hdr;

	ErrFatalDisplayIf(mh==NULL, "(DocOpen) Out of memory");
	d=MemHandleLock(mh);
	MemSet(d, sizeof(Document), 0);

	DBInit(&d->db, name, path);
	if (DBOpen(&d->db, dmModeReadOnly, true)==true) {
		StrNCopy(d->title, name, DocNameLength-1);

		rh=DBGetRecord(&d->db, 0);
		hdr=MemHandleLock(rh);
		d->version=hdr->version;
		d->records=hdr->records;
		MemHandleFree(rh);
		if ((d->version==1) || (d->version==2)) {
			if (BuildIndex(d)==true) {
				UInt16 pNo;
				UInt32 pip;

				rh=MemHandleNew(d->pageSize+1);
				ErrFatalDisplayIf(rh==NULL, "(DocOpen) Out of memory.");
				d->page=MemHandleLock(rh);

				if (pos>=d->length)
					d->position=0;
				else
					d->position=pos;

				DocPagePos(d, d->position, &pNo, &pip);
				DocGetPage(d, pNo);
				d->posInPage=pip;
				return d;
			}
		} else
			FrmAlert(aUnknownDocType);
	}

	MemHandleFree(mh);
	return NULL;
}

/*
 * BuildIndex
 *
 * Build the pages "index" - validate document as we go.
 *
 *  -> d		Document.
 *
 * Returns true if index was build, false otherwise.
 */
static Boolean
BuildIndex(Document *d)
{
	MemHandle mh=MemHandleNew(sizeof(UInt16)*(d->records+1)), rh;
	UInt32 length=0;
	UInt16 maxSize=0, recIdx;
	UInt16 *idx;

	ErrFatalDisplayIf(mh==NULL, "(BuildIndex) Out of memory");
	idx=MemHandleLock(mh);
	d->pageLengths=idx;

	*idx++=0;
	for (recIdx=1; recIdx<(d->records+1); recIdx++) {
		rh=DBGetRecord(&d->db, recIdx);
		if (d->version==2)
			*idx=Validate(MemHandleLock(rh), MemHandleSize(rh));
		else
			*idx=(UInt16)MemHandleSize(rh);

		MemHandleFree(rh);

		if (!*idx) {
			FrmAlert(aInvalidDocument);
			return false;
		}

		if (*idx>maxSize)
			maxSize=*idx;

		length+=*idx;
		DPrint("Page %u = %u\n", recIdx, *idx);
		idx++;
	}

	DPrint("Page maxSize = %u\n", maxSize);
	d->pageSize=maxSize;
	d->length=length;
	return true;
}

/*
 * Validate
 *
 * Decompress and validate a page.
 *
 *  -> src		Source page.
 *  -> len		Source length.
 *
 * Returns length of decompressed page or 0 (failure).
 */
static UInt16
Validate(UInt8 *src, UInt32 len)
{
	UInt8 *stop=(src+len);
	UInt16 dLen=0, m;
	UInt8 byte;

	while (src<stop) {
		byte=*src++;

		if ((byte>=1) && (byte<=8)) {
			dLen+=(UInt16)byte;
			src+=(UInt32)byte;
		} else if (byte<=0x7f) {
			dLen++;
		} else if (byte>=0xc0) {
			dLen+=2;
		} else {
			m=(((UInt16)byte)<<8)+(*src++);
			dLen+=(m&7)+3;
		}
	}

	return dLen;
}

/*
 * DocClose
 *
 * Close the document.
 *
 *  -> d		DocumentPtr;
 */
void
DocClose(Document **d)
{
	if (*d) {
		Document *doc=*d;

		if (doc->pageLengths)
			MemPtrFree(doc->pageLengths);

		if (doc->page)
			MemPtrFree(doc->page);

		DBClose(&doc->db);
		MemPtrFree(doc);
		*d=NULL;
	}
}

/*
 * DocPagePos
 *
 * Given a position return page number and position in page.
 *
 *  -> d		Document.
 *  -> pos		Position.
 * <-  pageNo		Page number.
 * <-  posInPage	Position in page.
 */
void
DocPagePos(Document *d, UInt32 pos, UInt16 *pageNo, UInt32 *posInPage)
{
	UInt16 idx;

	for (idx=1; idx<(d->records+1); idx++) {
		if (pos<(d->pageLengths[idx])) {
			*pageNo=idx;
			*posInPage=pos;
			return;
		}

		pos-=(UInt32)(d->pageLengths[idx]);
	}

	*pageNo=1;
	*posInPage=0;
}

/*
 * DocGetLine
 *
 * Get the next line from the document.
 *
 *  -> d		Document. 
 *  -> w		Display width.
 * <-  cnt		Where to store count of lines in this page.
 * <-  lineComplete	True if entire line was gotten.
 *
 * Returns true if a next line was fetched, false otherwise.
 */
Boolean
DocGetLine(Document *d, UInt16 w, UInt16 *cnt, Boolean *lineComplete)
{
	UInt16 pNo;

	if (d->position>=d->length)
		return false;

	DocPagePos(d, d->position, &pNo, &d->posInPage);
	DPrint("page: %u    pos: %lu", pNo, d->posInPage);

	if (pNo>(d->records)+1)
		return false;

	if (pNo!=d->pageNo)
		DocGetPage(d, pNo);

	*lineComplete=false;
	if ((*cnt=FntWordWrap(d->page+d->posInPage, w))>0) {
		d->position+=(UInt32)*cnt;
		if (d->posInPage+((UInt32)(*cnt))<d->pageLengths[pNo])
			*lineComplete=true;
	}

	return true;
}

/*
 * DocGetPage
 *
 * Fill buffer with page no.
 *
 *  -> d		Document.
 *  -> pNo		Page no.
 */
void
DocGetPage(Document *d, UInt16 pNo)
{
	MemHandle mh=DBGetRecord(&d->db, pNo);
	UInt8 *p;

	if (d->version==2)
		Decompress(d->page, d->pageLengths[pNo], MemHandleLock(mh), MemHandleSize(mh));
	else
		MemMove(d->page, MemHandleLock(mh), d->pageLengths[pNo]);

	MemHandleFree(mh);

	d->page[(UInt32)(d->pageLengths[pNo])]=0;
	d->pageNo=pNo;
	d->posInPage=0;

	for (p=d->page; *p; p++) {
		if ((*p<' ')) {
			switch (*p) {
			case '\t':
			case '\n':
			       break;
			case '\r':	/* FALL-THRU */
			default:
			       *p=' ';
			       break;
		       }
		}
	}
}

/*
 * Decompress
 *
 * Decompresses a version 2 doc.
 *
 *  -> dst		Where to store decompressed data.
 *  -> dstLen		Length of destination buffer.
 *  -> src		Compressed source data.
 *  -> len		How much data to decompress.
 */			
static void
Decompress(UInt8 *dst, UInt32 dstLen, UInt8 *src, UInt32 len)
{
	UInt32 idx=0, dLen=0;
	UInt8 byte;
	UInt16 m, di;
	Int16 n;

	while (idx<len) {
		byte=src[idx++];

		if ((byte>=1) && (byte<=8)) {
			while (byte--)
				dst[dLen++]=src[idx++];
		} else if (byte<=0x7f) {
			dst[dLen++]=byte;
		} else if (byte>=0xc0) {
			dst[dLen++]=' ';
			dst[dLen++]=byte^0x80;
		} else {
			m=(((UInt16)byte)<<8)+src[idx++];
			di=(m&0x3fff)>>3;
			for (n=(m&7)+3; n>0; n--) {
				dst[dLen]=dst[dLen-di];
				dLen++;
			}
		}
	}
}
