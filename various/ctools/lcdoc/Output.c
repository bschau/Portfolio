/**
 * Routines used to output the HTML files. This will create the various pages
 * and the index.html file.
 */
#ifdef Linux
#include <libgen.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "Include.h"

/* macros */
#define FilenameLength 32

/* structs */
struct Fode {
	struct Fode *next;
	char *bName;
	char fName[FilenameLength];
	int flags;
};
#define FEnumsSent 1
#define FStructsSent 2
#define FVarsSent 4
#define FFuncsSent 8
#define FTypeDefsSent 16

/* protos */
static struct Fode *FindFile(char *);
static struct Fode *AddFile(char *);
static void Section(struct Node *, int, char *);
static struct Fode *Setup(struct Node *, FILE **, int, char *);
static void EndUse(FILE **);
static void SubTable(FILE *, char *);
static void RunString(char *);
static void WriteHeaders(struct Node *);
static void HtmlOut(FILE *, char *);
static void AddToIndex(struct Node *);

/* globals */
static char *htmlHeader="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3c.org/TR/html4/strict.dtd\">\n<HTML>\n<HEAD>\n<TITLE>@@file@@</TITLE>\n<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=ISO-8859-1\">\n<STYLE TYPE=\"text/css\">\n@@style@@\n</STYLE>\n</HEAD>\n<BODY>\n<TABLE>\n<TR>\n<TD>&nbsp;</TD><TD><H1>@@file@@</H1></TD></TR>\n";
static char *htmlLeft="<TR><TD CLASS=\"left\"><A HREF=\"index.html\" CLASS=\"left\"><I>Index</I></A><BR>@@filelist@@</TD>";
static char *htmlRight="<TD CLASS=\"right\">\n";
static char *htmlFooter="</TD>\n</TR>\n</TABLE>\n</BODY>\n</HTML>\n";
static struct Fode *fileList=NULL;
static int nextFile=0;
static char *file, *tmp;
static struct Node *indexNodes;

/**
 * Find file on file list.
 *
 * @param n File name.
 * @return Fode or NULL.
 */
static struct Fode *
FindFile(char *n)
{
	struct Fode **prev=&fileList;
	struct Fode *l;

	while ((l=*prev)!=NULL) {
		if (strcmp(n, l->bName)==0) {
			return l;
		}

		prev=&l->next;
	}

	return NULL;
}

/**
 * Add file to list.
 *
 * @param n File name.
 * @return Node.
 */
static struct Fode *
AddFile(char *n)
{
	struct Fode *f=calloc(1, sizeof(struct Fode)), *l;
	struct Fode **prev=&fileList;

	if (!f) {
		XError(NULL, "OOM while allocating file node");
	}

	f->bName=n;

	nextFile++;
	snprintf(f->fName, FilenameLength, "doc%i.html", nextFile);

	while ((l=*prev)!=NULL) {
		if (strcasecmp(n, l->bName)<0) {
			f->next=l;
			*prev=f;
			return f;
		}

		prev=&l->next;
	}

	*prev=f;
	f->next=l;

	return f;
}

/**
 * Create HTML files.
 */
void
Output(void)
{
	FILE *fp=NULL;
	struct Node *n;
	struct Fode *f;

	WriteHeaders(gList);
	WriteHeaders(eList);
	WriteHeaders(sList);
	WriteHeaders(vList);
	WriteHeaders(fList);
	WriteHeaders(tList);

	RunString(htmlLeft);
	RunString(htmlRight);

	/* global text */
	for (n=gList; n; n=n->next) {
		f=Setup(n, &fp, 0, NULL);

		if (n->desc) {
			tmp=n->desc;
			HtmlOut(fp, "<P CLASS=\"global\">@@tmp@@");
		}
	}
	EndUse(&fp);

	Section(eList, FEnumsSent, "Enumerations");
	Section(sList, FStructsSent, "Structures");
	Section(tList, FTypeDefsSent, "Type Definitions");
	Section(vList, FVarsSent, "Variables");
	Section(fList, FFuncsSent, "Functions");

	RunString(htmlFooter);
}

/**
 * Dump section.
 *
 * @param n First node.
 * @param flag FFlag.
 * @param hdr Header.
 */
static void
Section(struct Node *n, int flag, char *hdr)
{
	FILE *fp=NULL;
	struct Fode *f;

	for (; n; n=n->next) {
		f=Setup(n, &fp, flag, hdr);

		HtmlOut(fp, "<A NAME=\"@@tmp@@\"></A><P CLASS=\"entry\">@@tmp@@");

		if (n->signature) {
			tmp=n->signature;
			HtmlOut(fp, "<DIV CLASS=\"signature\"><PRE>@@tmp@@</PRE></DIV>");
		}

		if (n->desc) {
			tmp=n->desc;
			HtmlOut(fp, "<P CLASS=\"desc\">@@tmp@@");
		}

		if (n->param) {
			HtmlOut(fp, "<P CLASS=\"right\">Parameters:");
			SubTable(fp, n->param);
		}

		if (n->ret) {
			tmp=n->ret;
			HtmlOut(fp, "<P CLASS=\"right\">Returns:<P CLASS=\"npr\">@@tmp@@");
		}

		if (n->note) {
			tmp=n->note;
			HtmlOut(fp, "<P CLASS=\"right\">Note:<P CLASS=\"npr\">@@tmp@@");
		}
	}
	EndUse(&fp);
}

/**
 * Setup for html output.
 *
 * @param n Node.
 * @param fp Where to store opened file pointer.
 * @param ht Header type.
 * @param hdr Header.
 * @return Fode.
 */
static struct Fode *
Setup(struct Node *n, FILE **fp, int ht, char *hdr)
{
	struct Fode *f;
	char *name;

	if (*fp) {
		fclose(*fp);
		*fp=NULL;
	}

	name=n->key+strlen(n->key)+1;
	if ((f=FindFile(name))==NULL) {
		XError("File '%s' disappeared from list!?!?", name);
	}

	if ((*fp=fopen(f->fName, "ab+"))==NULL) {
		XError("Failed to open '%s'", f->fName);
	}

	if (hdr) {
		if (!(f->flags&ht)) {
			tmp=hdr;
			HtmlOut(*fp, "<H3>@@tmp@@</H3>\n<P CLASS=\"right\">");
			f->flags|=ht;
		}
	}
	
	file=f->bName;
	if (*n->key) {
		tmp=n->key;
	} else {
		tmp="Unnamed";
	}

	return f;
}

/**
 * End usage of fp.
 *
 * @param fp Where fp is stored.
 */
static void
EndUse(FILE **fp)
{
	if (*fp) {
		fclose(*fp);
		*fp=NULL;
	}
}

/**
 * Write a subtable (which, in fact, isn't a table at all!)
 *
 * @param fp File pointer.
 * @param src Source data.
 */
static void
SubTable(FILE *fp, char *src)
{
	char c;

	HtmlOut(fp, "<DIV CLASS=\"npr\">");
	for (EVER) {
		for (tmp=src; *src && *src!='\t'; src++);
		if (tmp==src) {
			break;
		}

		*src=0;
		tmp=UTrim(tmp);
		HtmlOut(fp, "<CODE>@@tmp@@</CODE>");
		for (tmp=++src; *src && *src!='\n'; src++);
		c=*src;
		*src=0;
		tmp=UTrim(tmp);
		HtmlOut(fp, " - @@tmp@@<BR>\n");
		if (!c) {
			break;
		} else {
			src++;
		}
	}
	HtmlOut(fp, "</DIV>\n");
}

/**
 * Output string to all files.
 *
 * @param src Source html string.
 */
static void
RunString(char *src)
{
	struct Fode *f;
	FILE *fp;

	for (f=fileList; f; f=f->next) {
		if ((fp=fopen(f->fName, "ab+"))==NULL) {
			XError("Failed to open '%s'", f->fName);
		}
	
		file=f->bName;
		HtmlOut(fp, src);
		fclose(fp);
	}
}

/**
 * Write the headers for the files on this list (if not already present).
 *
 * @param n First node.
 */
static void
WriteHeaders(struct Node *n)
{
	struct Fode *f;
	char *b;
	FILE *fp;

	while (n) {
		if ((b=strchr(n->key, 1))) {
			*b++=0;
			if ((f=FindFile(b))==NULL) {
				f=AddFile(b);

				if ((fp=fopen(f->fName, "wb+"))==NULL) {
					XError("Failed to open '%s'", f->fName);
				}
	
				file=f->bName;
				HtmlOut(fp, htmlHeader);
				fclose(fp);
			}
		}

		n=n->next;
	}
}

/**
 * Write formatted Html output.
 *
 * @param fp File pointer.
 * @param html Html source string.
 */
static void
HtmlOut(FILE *fp, char *html)
{
	int len;
	char *p;

	while ((p=UStrInStr(html, "@@"))) {
		if ((len=p-html)) {
			if (fwrite(html, len, 1, fp)!=1) {
				XError(NULL, "Failed to write HTML output");
			}
		}

		if (strncasecmp(p, "@@file@@", 8)==0) {
			fprintf(fp, "%s", basename(file));
			p+=8;
		} else if (strncasecmp(p, "@@style@@", 9)==0) {
			fprintf(fp, "%s", styleSheet);
			p+=9;
		} else if (strncasecmp(p, "@@filelist@@", 12)==0) {
			int pre=0;
			struct Fode *f;

			for (f=fileList; f; f=f->next) {
				if (strcasecmp(file, f->bName)==0) {
					fprintf(fp, "%s<A HREF=\"%s\" CLASS=\"selected\">%s</A>\n", (pre ? "<BR>" : ""), f->fName, basename(f->bName));
				} else {
					fprintf(fp, "%s<A HREF=\"%s\" CLASS=\"left\">%s</A>\n", (pre ? "<BR>" : ""), f->fName, basename(f->bName));
				}

				pre=1;
			}

			p+=12;
		} else if (strncasecmp(p, "@@tmp@@", 7)==0) {
			fprintf(fp, "%s", tmp);
			p+=7;
		}

		if (ferror(fp)) {
			XError(NULL, "Failed to write HTML output");
		}

		html=p;
	}

	if (*html) {
		fprintf(fp, "%s", html);
	}

	if (ferror(fp)) {
		XError(NULL, "Failed to write HTML output");
	}
}

/**
 * Generate index page.
 */
void
Index(void)
{
	FILE *fp=NULL;
	struct Node *n;
	struct Fode *f;
	char *p;

	if ((fp=fopen("index.html", "wb+"))==NULL) {
		XError(NULL, "Failed to open index.html");
	}

	file="Index";
	HtmlOut(fp, htmlHeader);
	HtmlOut(fp, htmlLeft);
	HtmlOut(fp, htmlRight);

	XOut(XODebug, "Sorting index list");
	indexNodes=NULL;
	AddToIndex(eList);
	AddToIndex(sList);
	AddToIndex(vList);
	AddToIndex(fList);
	AddToIndex(tList);

	for (n=indexNodes; n; n=n->next) {
		tmp=n->key;
		p=n->key+strlen(n->key)+1;
		if ((f=FindFile(p))==NULL) {
			XError("File '%s' disappeared from list!?!?", p);
		}

		file=f->fName;
		HtmlOut(fp, "<A HREF=\"@@file@@#@@tmp@@\" CLASS=\"index\">@@tmp@@</A><BR>");
	}

	HtmlOut(fp, htmlFooter);
	fclose(fp);
}

/**
 * Add the list starting with this node to the index list. Entries are sorted.
 *
 * @param n First node.
 */
static void
AddToIndex(struct Node *n)
{
	struct Node **prev=&indexNodes;
	struct Node *i, *l;
	int c, done;

	for (; n; n=n->next) {
		i=UAlloc(NULL, sizeof(struct Node));
		i->key=n->key;
		i->desc=n->desc;
		i->signature=n->signature;
		i->param=n->param;
		i->ret=n->ret;
		i->note=n->note;

		done=0;
		while (((l=*prev)!=NULL) && (!done)) {
			if ((c=strcasecmp(i->key, l->key))<0) {
				i->next=l;
				*prev=i;
				done=1;
			} else {
				prev=&l->next;
			}
		}

		if (!done) {
			*prev=i;
			i->next=l;
		}
	}
}
