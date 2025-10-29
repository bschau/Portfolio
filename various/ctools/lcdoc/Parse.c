/**
 * <P>Main parser. Parse and initialize procedures, variables et. al.
 *
 * <P>The parser looks for comment blocks starting with &#47;&#42;&#42; (slash-
 * star-star) (like in Java). If the sequence is followed by a dash the comment
 * is ignored.
 * <P>Then follows the description and one or more of the keywords describing
 * the various input and output parameters for the function.
 * <P>The comment block must be terminated with &#42;&#47; (star-slash).
 * <P>After the comment block the signature of what is describe follows.
 * <I>lcdoc</I> tries to auto guess the type being describe (function, structs,
 * typedefs, enums or variables). If <I>lcdoc</I> parses the type incorrectly,
 * you can force the type using a directive in the comment block.
 *
 * <P>Directives:
 *
 * <DL>
 * <DT>&#64;param</DT>
 * <DD>A parameter for a function.</DD>
 * <DT>&#64;member</DT>
 * <DD>A member in a enum or structure. <I>member</I> and <I>param</I> are
 * interchangeable.</DD>
 * <DT>&#64;return</DT>
 * <DD>What kind of value (if any) this function returns.</DD>
 * <DT>&#64;note</DT>
 * <DD>Something to be hightlighted.</DD>
 * <DT>&#64;type</DT>
 * <DD>Set to <B>struct</B>, <B>enum</B>, <B>var</B>, <B>func</B> or
 * <B>typedef</B> to force the target to the specified type.</DD>
 * </DL>
 */
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "Include.h"

/* macros */
enum { MDesc=0, MReturn, MNote, MParamP, MParamDesc, MType };
enum { SigUndef=0, SigNoSig, SigFunc, SigVar, SigStruct, SigEnum, SigTypeDef };

/* protos */
static void FreeList(struct Node *);
static void Extract(char *);
static int Remode(char *, int, int *);
static char *ParseObject(char *);
static char *SkipWhite(char *);
static int SigType(void);
static void Spacer(StringBuilder, int);
static void FunctionGet(char **, char **);
static void VarGet(char **, char **);
static void StructGet(char **, char **);
static void EnumGet(char **, char **);
static void TypeDefGet(char **, char **);
static struct Node *BuildNode(char *, char *);
static void AddNode(struct Node *);

/* globals */
static StringBuilder sigBuilder, descBuilder, paramBuilder, noteBuilder;
static StringBuilder returnBuilder, typeBuilder;
static char *mem;
static int sigType;

/**
 * Initialize parser.
 */
void
ParseOpen(void)
{
	XOut(XODebug, "Allocating parser resources");
	sigBuilder=SBNew(4096);
	descBuilder=SBNew(4096);
	paramBuilder=SBNew(1024);
	noteBuilder=SBNew(1024);
	returnBuilder=SBNew(1024);
	typeBuilder=SBNew(1024);
}

/**
 * Destroy parser.
 */
void
ParseClose(void)
{
	XOut(XODebug, "Deallocating parser");
	FreeList(gList);
	FreeList(eList);
	FreeList(sList);
	FreeList(vList);
	FreeList(fList);
	FreeList(tList);
	SBFree(typeBuilder);
	SBFree(returnBuilder);
	SBFree(noteBuilder);
	SBFree(paramBuilder);
	SBFree(descBuilder);
	SBFree(sigBuilder);
}

/**
 * Free a list given by ptr.
 *
 * @param ptr First node (or NULL).
 */
static void
FreeList(struct Node *ptr)
{
	struct Node *t;

	while (ptr) {
		t=ptr->next;
		free(ptr);
		ptr=t;
	}
}

/**
 * Setup for new file parse.
 *
 * @param name File name.
 */
void
ParseNew(char *name)
{
	struct stat sBuf;
	FILE *fp;
	
	XOut(XODebug, "Loading '%s'", name);
	currentFile=NULL;
	if (stat(name, &sBuf)) {
		XError("Failed to stat '%s'", name);
	}

	mem=UAlloc(NULL, sBuf.st_size+8);

	if ((fp=fopen(name, "rb"))==NULL) {
		XError("Failed to open '%s'", name);
	}

	if (fread(mem, sBuf.st_size, 1, fp)!=1) {
		XError("Failed to read '%s'", name);
	}

	fclose(fp);
	currentFile=name;
	lineNo=1;
}

/**
 * Close down file parsing for this file.
 */
void
ParseFree(void)
{
	if (mem) {
		free(mem);
		mem=NULL;
	}

	currentFile=NULL;
}

/**
 * Parse a file.
 */
void
Parse(void)
{
	char *s, *cmt, *name, *signature;
	int cmtStart;
	struct Node *node;

	XOut(XODebug, "Parsing '%s'", currentFile);
	for (s=mem; *s; s++) {
		if (*s=='\n') {
			lineNo++;
		}

		if ((*s=='/') && (*(s+1)=='*') && (*(s+2)=='*')) {
			XOut(XODebug, "  Comment found on line %i", lineNo);
			s+=3;
			if (*s=='-') {
				XOut(XODebug, "   Ignoring");
				continue;
			}

			cmtStart=lineNo;
			cmt=s;
			for (; *s; s++) {
				if ((*s=='*') && (*(s+1)=='/')) {
					break;
				}

				if (*s=='\n') {
					lineNo++;
				}
			}

			if (!*s) {
				XError(NULL, "Unterminated comment starting on line %i", cmtStart);
			}

			*s=0;
			s+=2;

			sigType=SigUndef;
			name=NULL;
			signature=NULL;
			SBReset(descBuilder);
			SBReset(paramBuilder);
			SBReset(noteBuilder);
			SBReset(returnBuilder);
			SBReset(sigBuilder);
			SBReset(typeBuilder);

			XOut(XODebug, "  Signature found on line %i", lineNo);
			Extract(cmt);

			if ((s=ParseObject(s))!=NULL) {
				if (sigType==SigUndef) {
					sigType=SigType();
				}

				XOut(XODebug, "  Signature: (%i) %s", sigType, SBToString(typeBuilder));

				if (sigType>SigNoSig) {
					switch (sigType) {
					case SigFunc:
						FunctionGet(&name, &signature);
						break;
					case SigVar:
						VarGet(&name, &signature);
						break;
					case SigStruct:
						StructGet(&name, &signature);
						break;
					case SigEnum:
						EnumGet(&name, &signature);
						break;
					case SigTypeDef:
						TypeDefGet(&name, &signature);
						break;
					default:
						XError(NULL, "Unknown signature type");
					}

					if (!addStatics) {
						if (strncmp(signature, "static", 6)==0) {
							if (signature) {
								free(signature);
							}

							free(name);
							continue;
						}
					}

					if (!name) {
						name=UStrDup(NULL, " ");
					}
				} else {
					name=UStrDup(NULL, " ");
					signature=NULL;
				}
				s--;

				node=BuildNode(name, signature);
				AddNode(node);

				if (signature) {
					free(signature);
				}
	
				free(name);
			} else {
				s--;
			}
		}
	}
}

/**
 * Extract info from comment block.
 *
 * @param src Source comment block.
 */
static void
Extract(char *src)
{
	int mode=MDesc, pPre=0, len;
	char save;
	char *t;

	for (EVER) {
		for (; *src && isspace((int)*src); src++);
		if (!*src) {
			break;
		}

		if (*src=='*') {
			src++;
			continue;
		}

		for (t=src; *src && !isspace((int)*src); src++);
		save=*src;
		*src=0;
		len=src-t;

		switch (mode) {
		case MReturn:
			if (!Remode(t, len, &mode)) {
				SBAppend(returnBuilder, t, len);
				SBAppendChar(returnBuilder, ' ');
			}
			break;
		case MNote:
			if (!Remode(t, len, &mode)) {
				SBAppend(noteBuilder, t, len);
				SBAppendChar(noteBuilder, ' ');
			}
			break;
		case MParamP:
			if (!Remode(t, len, &mode)) {
				if (pPre) {
					SBAppendChar(paramBuilder, '\n');
				} else {
					pPre=1;
				}

				SBAppend(paramBuilder, t, len);
				SBAppendChar(paramBuilder, '\t');
				mode=MParamDesc;
			}
			break;
		case MParamDesc:
			if (!Remode(t, len, &mode)) {
				SBAppend(paramBuilder, t, len);
				SBAppendChar(paramBuilder, ' ');
			}
			break;
		case MType:
			if (!Remode(t, len, &mode)) {
				if (strncasecmp(t, "struct", 6)==0) {
					sigType=SigStruct;
				} else if (strncasecmp(t, "enum", 4)==0) {
					sigType=SigEnum;
				} else if (strncasecmp(t, "var", 3)==0) {
					sigType=SigVar;
				} else if (strncasecmp(t, "func", 4)==0) {
					sigType=SigFunc;
				} else if (strncasecmp(t, "typedef", 7)==0) {
					sigType=SigTypeDef;
				} else {
					XError("Unknown signature type: '%s'", t);
				}
			}
			break;

		default:
			if (!Remode(t, len, &mode)) {
				SBAppend(descBuilder, t, len);
				SBAppendChar(descBuilder, ' ');
			}
			break;
		}

		*src=save;
	}
}

/**
 * See if mode changes.
 *
 * @param t Token.
 * @param len Token length.
 * @param mode Where to get/store mode.
 * @return 0 if mode wasn't changed, !0 otherwise.
 */
static int
Remode(char *t, int len, int *mode)
{
	int m=*mode;

	if (strncasecmp(t, "@param", len)==0) {
		m=MParamP;
	} else if (strncasecmp(t, "@member", len)==0) {
		m=MParamP;
	} else if (strncasecmp(t, "@return", len)==0) {
		m=MReturn;
	} else if (strncasecmp(t, "@note", len)==0) {
		m=MNote;
	} else if (strncasecmp(t, "@type", len)==0) {
		m=MType;
	} else {
		return 0;
	}

	if (*mode==m) {
		return 0;
	}

	*mode=m;
	return 1;
}

/**
 * Skip whitespaces, update line counter.
 *
 * @param s String.
 * @return Updated pointer.
 */
static char *
SkipWhite(char *s)
{
	while (*s) {
		if (*s>' ') {
			return s;
		}

		if (*s=='\n') {
			lineNo++;
		}

		s++;
	}

	return NULL;
}

/**
 * Parse the "object" starting at s.
 *
 * @param s Start of object.
 * @return New s position or NULL if EOF.
 */
static char *
ParseObject(char *s)
{
	int level=0, blockSeen=0;

	if ((s=SkipWhite(s))==NULL) {
		return NULL;
	}

	for (; *s; s++) {
		if (*s=='\n') {
			lineNo++;
			*s=' ';
		} else if ((*s=='/') && (*(s+1)=='*') && (*(s+2)=='*')) {
			break;
		} else if (*s=='{') {
			level++;
			blockSeen=1;
		} else if (*s=='}') {
			level--;
		} else if (*s==';' && !level) {
			SBAppendChar(sigBuilder, ';');
			break;
		} else if (*s<' ') {
			continue;
		}

		SBAppendChar(sigBuilder, *s);
	}

	*(s-1)=0;

	return s;
}

/**
 * Parse the signature.
 */
static int
SigType(void)
{
	int level=0;
	char c=-1, term;
	char *sig, *p;

	SBAppendChar(typeBuilder, ' ');
	for (sig=SBToString(sigBuilder); *sig; sig++) {
		if (*sig=='\'' || *sig=='"') {
			term=*sig;
			for (sig++; *sig && *sig!=term; sig++);
			if (*sig) {
				sig++;
			}
		} else if (isspace((int)*sig) && !level) {
			if (c!=' ') {
				SBAppendChar(typeBuilder, ' ');
				c=' ';
			}
		} else if (*sig=='{') {
			level++;
			c='{';
		} else if (*sig=='}') {
			level--;
			c='}';
		} else {
			if (!level) {
				SBAppendChar(typeBuilder, *sig);
				c=*sig;
			}
		}
	}

	sig=SBToString(typeBuilder);
	if (*(sig+1)=='#') {
		return SigNoSig;
	}

	if ((p=strchr(sig, '#'))) {
		*p=0;
	}

	if (UStrInStr(sig, " typedef ")) {
		return SigTypeDef;
	}

	if (strchr(sig, '(')) {
		if (strchr(sig, ';')) {
			return SigVar;
		}

		return SigFunc;
	}

	if (UStrInStr(sig, " enum ")) {
		return SigEnum;
	}

	if (UStrInStr(sig, " struct ")) {
		return SigStruct;
	}

	return SigVar;
}

/**
 * Emit X spaces.
 *
 * @param cnt Number of spaces to emit.
 */
static void
Spacer(StringBuilder s, int cnt)
{
	cnt*=8;
	for (; cnt>0; cnt--) {
		SBAppendChar(s, ' ');
	}
}

/**
 * Get function signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void
FunctionGet(char **name, char **signature)
{
	char *sig=UTrim(SBToString(typeBuilder)), *p;
	int level=0;
	char last=' ';

	SBReset(sigBuilder);
	for (p=sig; *p; p++) {
		if (*p=='(') {
			if (last!=' ') {
				SBAppendChar(sigBuilder, ' ');
			}

			level++;
			SBAppend(sigBuilder, "(<BR>", 5);
			Spacer(sigBuilder, level);
			last=' ';
		} else if (*p==')') {
			SBAppend(sigBuilder, "<BR>)", 5);
			level--;
			if (!level) {
				break;
			}
			last=')';
		} else if (isspace((int)*p)) {
			if (last!=' ') {
				SBAppendChar(sigBuilder, ' ');
				last=' ';
			}
		} else if (*p==',') {
			SBAppend(sigBuilder, ",<BR>", 5);
			Spacer(sigBuilder, level);
			last=' ';
		} else {
			SBAppendChar(sigBuilder, *p);
			last=*p;
		}
	}

	sig=UTrim(SBToString(sigBuilder));
	*signature=UStrDup(NULL, sig);
	p=SBToString(sigBuilder)+SBLength(sigBuilder);
	for (--p; p>sig; p--) {
		if (*p==')') {
			level++;
		} else if (*p=='(') {
			level--;
			if (!level) {
				break;
			}
		}
	}

	for (--p; p>sig && isspace((int)*p); p--);
	*(p+1)=0;
	for (; p>sig && !isspace((int)*p); p--);
	*name=UStrDup(NULL, UTrim(++p));
}

/**
 * Get variable signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void
VarGet(char **name, char **signature)
{
	char *sig=UTrim(SBToString(sigBuilder)), *p;
	char last=' ';

	SBReset(typeBuilder);
	for (p=sig; *p; p++) {
		if (isspace((int)*p)) {
			if (last!=' ') {
				last=' ';
				SBAppendChar(typeBuilder, ' ');
			}
		} else {
			if (*p==';') {
				break;
			}

			SBAppendChar(typeBuilder, *p);
			last=*p;
		}
	}

	sig=UTrim(SBToString(typeBuilder));
	*signature=UStrDup(NULL, sig);

	if ((p=strchr(sig, '['))==NULL) {
		if ((p=strchr(sig, ';'))==NULL) {
			p=sig+SBLength(typeBuilder)-1;
		}
	} else {
		*p=0;
		p--;
	}

	for (; p>sig && isspace((int)*p); p--);

	for (; p>sig && !isspace((int)*p); p--);
	p++;
	if ((sig=strchr(p, '='))) {
		*sig=0;
	}

	*name=UStrDup(NULL, UTrim(p));
}

/**
 * Get structure signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void
StructGet(char **name, char **signature)
{
	char *sig=UTrim(SBToString(sigBuilder)), *p, *t;
	int level=0;
	char last=' ';

	SBReset(typeBuilder);
	for (p=sig; *p; p++) {
		if (isspace((int)*p)) {
			if (last!=' ') {
				last=' ';
				SBAppendChar(typeBuilder, ' ');
			}
		} else if (*p=='{') {
			if (last!=' ') {
				SBAppendChar(typeBuilder, ' ');
			}

			SBAppend(typeBuilder, "{<BR>", 5);
			level++;
			Spacer(typeBuilder, level);
			last=' ';
		} else if (*p=='}') {
			SBAppend(typeBuilder, "}<BR>", 5);
			level--;
			if (!level) {
				break;
			}
			last='}';
		} else if (*p==';') {
			SBAppend(typeBuilder, ";<BR>", 5);
			t=p;
			while (isspace((int)*(t+1))) {
				t++;
			}

			if (*(t+1)!='}') {
				Spacer(typeBuilder, level);
			}

			last=' ';
		} else {
			SBAppendChar(typeBuilder, *p);
			last=*p;
		}
	}

	sig=UTrim(SBToString(typeBuilder));
	*signature=UStrDup(NULL, sig);

	for (p=sig; *p && *p!='{'; p++);
	for (--p; p>sig && isspace((int)*p); p--);
	*(p+1)=0;
	for (; p>sig && !isspace((int)*p); p--);
	*name=UStrDup(NULL, p+1);
}

/**
 * Get enum signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void
EnumGet(char **name, char **signature)
{
	char *sig=UTrim(SBToString(sigBuilder)), *p, *t;
	int level=0;
	char last=' ';

	SBReset(typeBuilder);
	for (p=sig; *p; p++) {
		if (isspace((int)*p)) {
			if (last!=' ') {
				last=' ';
				SBAppendChar(typeBuilder, ' ');
			}
		} else if (*p=='{') {
			if (last!=' ') {
				SBAppendChar(typeBuilder, ' ');
			}

			SBAppend(typeBuilder, "{<BR>", 5);
			level++;
			Spacer(typeBuilder, level);
			last=' ';
		} else if (*p=='}') {
			t=p;
			while (isspace((int)*(t-1))) {
				t--;
			}

			if (*t==',') {
				SBAppend(typeBuilder, "}<BR>", 5);
			} else {
				SBAppend(typeBuilder, "<BR>}<BR>", 9);
			}

			level--;
			if (!level) {
				break;
			}

			last='}';
		} else if (*p==',') {
			SBAppend(typeBuilder, ",<BR>", 5);
			t=p;
			while (isspace((int)*(t+1))) {
				t++;
			}

			if (*(t+1)!='}') {
				Spacer(typeBuilder, level);
			}

			last=' ';
		} else {
			SBAppendChar(typeBuilder, *p);
			last=*p;
		}
	}

	sig=UTrim(SBToString(typeBuilder));
	*signature=UStrDup(NULL, sig);

	p=UStrInStr(sig, "enum");
	for (p+=4; *p && isspace((int)*p); p++);
	if (*p!='{') {
		sig=p;
		for (sig=p; *p && !isspace((int)*p); p++);
		*p=0;
		*name=UStrDup(NULL, sig);
	} else {
		*name=NULL;
	}
}

/**
 * Get typedef signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void
TypeDefGet(char **name, char **signature)
{
	char *sig=UTrim(SBToString(sigBuilder)), *p, *t;
	int level=0;
	char last=' ';

	SBReset(typeBuilder);
	for (p=sig; *p; p++) {
		if (isspace((int)*p)) {
			if (last!=' ') {
				last=' ';
				SBAppendChar(typeBuilder, ' ');
			}
		} else if (*p=='{') {
			if (last!=' ') {
				SBAppendChar(typeBuilder, ' ');
			}

			SBAppend(typeBuilder, "{<BR>", 5);
			level++;
			Spacer(typeBuilder, level);
			last=' ';
		} else if (*p=='}') {
			last='}';
			level--;
			if (!level) {
				SBAppendChar(typeBuilder, '}');
				p++;
				break;
			} else {
				SBAppend(typeBuilder, "}<BR>", 5);
			}
		} else if (*p==';') {
			if (level) {
				SBAppend(typeBuilder, ";<BR>", 5);
				t=p;
				while (isspace((int)*(t+1))) {
					t++;
				}

				if (*(t+1)!='}') {
					Spacer(typeBuilder, level);
				}

				last=' ';
			} else {
				break;
			}
		} else if (*p==',') {
			SBAppend(typeBuilder, ",<BR>", 5);
			t=p;
			while (isspace((int)*(t+1))) {
				t++;
			}

			if (*(t+1)!='}') {
				Spacer(typeBuilder, level);
			}

			last=' ';
		} else {
			SBAppendChar(typeBuilder, *p);
			last=*p;
		}
	}

	for (; *p && *p!=';'; p++) {
		if (isspace((int)*p)) {
			if (last!=' ') {
				SBAppendChar(typeBuilder, ' ');
				last=' ';
			}
		} else {
			SBAppendChar(typeBuilder, *p);
			last=*p;
		}
	}

	sig=UTrim(SBToString(typeBuilder));
	*signature=UStrDup(NULL, sig);

	for (p=sig; *p; p++);
	--p;
	if (*p==';') {
		--p;
	}

	for (; p>sig && isspace((int)*p); p--);
	*(p+1)=0;
	for (; p>sig && !isspace((int)*p); p--);
	*name=UStrDup(NULL, p+1);
}

/**
 * Build a new node.
 *
 * @param key Key.
 * @param sig Function signature (or NULL).
 * @return Node.
 */
static struct Node *
BuildNode(char *key, char *sig)
{
	int size=sizeof(struct Node), sigLen=0, fLen, kLen, dLen, pLen, nLen, rLen;
	char *dP, *pP, *nP, *rP, *p;
	struct Node *n;

	fLen=strlen(currentFile)+1;
	size+=fLen;

	if (sig) {
		sigLen=strlen(sig);
		size+=sigLen+1;
	}

	key=UTrim(key);
	kLen=strlen(key);
	size+=kLen+1;

	dP=UTrim(SBToString(descBuilder));
	if ((dLen=strlen(dP))) {
		dLen++;
		size+=dLen;
	}

	pP=UTrim(SBToString(paramBuilder));
	if ((pLen=strlen(pP))) {
		pLen++;
		size+=pLen;
	}

	nP=UTrim(SBToString(noteBuilder));
	if ((nLen=strlen(nP))) {
		nLen++;
		size+=nLen;
	}

	rP=UTrim(SBToString(returnBuilder));
	if ((rLen=strlen(rP))) {
		rLen++;
		size+=rLen;
	}

	n=UAlloc(NULL, size);

	p=((char *)n)+sizeof(struct Node);
	n->key=p;
	memmove(p, key, kLen);
	p+=kLen;
	*p++=1;
	memmove(p, currentFile, fLen);
	p+=fLen;

	if (dLen) {
		n->desc=p;
		memmove(p, dP, dLen);
		p+=dLen;
	}

	if (pLen) {
		n->param=p;
		memmove(p, pP, pLen);
		p+=pLen;
	}

	if (rLen) {
		n->ret=p;
		memmove(p, rP, rLen);
		p+=rLen;
	}

	if (nLen) {
		n->note=p;
		memmove(p, nP, nLen);
		p+=nLen;
	}

	if (sigLen) {
		n->signature=p;
		memmove(p, sig, sigLen);
	}

	return n;
}

/**
 * Insert node alphabetically.
 *
 * @param n Node.
 */
static void
AddNode(struct Node *n)
{
	struct Node **prev;
	struct Node *l;
	int c;

	switch (sigType) {
	case SigFunc:
		prev=&fList;
		break;
	case SigVar:
		prev=&vList;
		break;
	case SigStruct:
		prev=&sList;
		break;
	case SigEnum:
		prev=&eList;
		break;
	case SigTypeDef:
		prev=&tList;
		break;
	default:
		prev=&gList;
		break;
	}
		
	while ((l=*prev)!=NULL) {
		if ((c=strcasecmp(n->key, l->key))<0) {
			n->next=l;
			*prev=n;
			return;
		}

		if (!c) {
			while ((l=*prev)!=NULL) {
				if (strcasecmp(n->key, l->key)==0) {
					prev=&l->next;
				} else {
					break;
				}
			}
			*prev=n;
			n->next=l;
			return;
		}

		prev=&l->next;
	}

	*prev=n;
	n->next=l;
}
