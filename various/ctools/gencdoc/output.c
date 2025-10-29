/**
 * Routines used to output the HTML files. This will create the various pages
 * and the index.html file.
 */
#ifndef WIN32
#include <libgen.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include.h"

/* macros */
#define FILENAME_LEN 32

/* structs */
struct fode {
	struct fode *next;
	char *b_name;
	char f_name[FILENAME_LEN];
	int flags;
};
#define F_DEFINES_SENT 1
#define F_ENUMS_SENT 2
#define F_STRUCTS_SENT 4
#define F_VARS_SENT 8
#define F_FUNCS_SENT 16
#define F_TYPEDEFS_SENT 32

/* protos */
static struct fode *find_file(char *n);
static struct fode *add_file(char *n);
static void section(struct node *n, int flag, char *hdr);
static struct fode *setup(struct node *n, FILE **fp, int ht, char *hdr);
static void end_use(FILE **fp);
static void sub_table(FILE *fp, char *src);
static void run_string(char *src);
static void write_headers(struct node *n);
static void html_out(FILE *fp, char *html);
static void add_to_index(struct node *n);

/* globals */
static char *html_header="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3c.org/TR/html4/strict.dtd\">\n<HTML>\n<HEAD>\n<TITLE>@@file@@</TITLE>\n<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=ISO-8859-1\">\n<STYLE TYPE=\"text/css\">\n@@style@@\n</STYLE>\n</HEAD>\n<BODY>\n<TABLE>\n<TR>\n<TD>&nbsp;</TD><TD><H1>@@file@@</H1></TD></TR>\n";
static char *html_left="<TR><TD CLASS=\"left\"><A HREF=\"index.html\" CLASS=\"left\"><I>Index</I></A><BR>@@filelist@@</TD>";
static char *html_right="<TD CLASS=\"right\">\n";
static char *html_footer="</TD>\n</TR>\n</TABLE>\n</BODY>\n</HTML>\n";
static struct fode *file_list=NULL;
static int next_file=0;
static char *file, *tmp;
static struct node *index_nodes;

/**
 * Find file on file list.
 *
 * @param n File name.
 * @return fode or NULL.
 */
static struct fode * find_file(char *n) {
	struct fode **prev = &file_list;
	struct fode *l;

	while ((l = *prev) != NULL) {
		if (strcmp(n, l->b_name) == 0) {
			return l;
		}

		prev = &l->next;
	}

	return NULL;
}

/**
 * Add file to list.
 *
 * @param n File name.
 * @return node.
 */
static struct fode *add_file(char *n) {
	struct fode *f = calloc(1, sizeof(struct fode)), *l;
	struct fode **prev = &file_list;

	if (!f) {
		u_error("OOM while allocating file node");
	}

	f->b_name = n;

	next_file++;
	snprintf(f->f_name, FILENAME_LEN, "doc%i.html", next_file);

	while ((l = *prev) != NULL) {
		if (strcasecmp(n, l->b_name) < 0) {
			f->next = l;
			*prev = f;
			return f;
		}

		prev = &l->next;
	}

	*prev = f;
	f->next = l;

	return f;
}

/**
 * Create HTML files.
 */
void output(void) {
	FILE *fp = NULL;
	struct node *n;

	write_headers(g_list);
	write_headers(e_list);
	write_headers(s_list);
	write_headers(v_list);
	write_headers(f_list);
	write_headers(t_list);

	run_string(html_left);
	run_string(html_right);

	/* global text */
	for (n = g_list; n; n = n->next) {
		setup(n, &fp, 0, NULL);

		if (n->desc) {
			tmp = n->desc;
			html_out(fp, "<P CLASS=\"global\">@@tmp@@");
		}
	}

	end_use(&fp);

	section(e_list, F_ENUMS_SENT, "Enumerations");
	section(s_list, F_STRUCTS_SENT, "Structures");
	section(t_list, F_TYPEDEFS_SENT, "Type Definitions");
	section(v_list, F_VARS_SENT, "Variables");
	section(f_list, F_FUNCS_SENT, "Functions");

	run_string(html_footer);
}

/**
 * Dump section.
 *
 * @param n First node.
 * @param flag FFlag.
 * @param hdr Header.
 */
static void section(struct node *n, int flag, char *hdr) {
	FILE *fp = NULL;

	for (; n; n = n->next) {
		setup(n, &fp, flag, hdr);

		html_out(fp, "<A NAME=\"@@tmp@@\"></A><P CLASS=\"entry\">@@tmp@@");

		if (n->signature) {
			tmp = n->signature;
			html_out(fp, "<DIV CLASS=\"signature\"><PRE>@@tmp@@</PRE></DIV>");
		}

		if (n->desc) {
			tmp = n->desc;
			html_out(fp, "<P CLASS=\"desc\">@@tmp@@");
		}

		if (n->param) {
			html_out(fp, "<P CLASS=\"right\">Parameters:");
			sub_table(fp, n->param);
		}

		if (n->ret) {
			tmp = n->ret;
			html_out(fp, "<P CLASS=\"right\">Returns:<P CLASS=\"npr\">@@tmp@@");
		}

		if (n->note) {
			tmp = n->note;
			html_out(fp, "<P CLASS=\"right\">Note:<P CLASS=\"npr\">@@tmp@@");
		}
	}

	end_use(&fp);
}

/**
 * setup for html output.
 *
 * @param n node.
 * @param fp Where to store opened file pointer.
 * @param ht Header type.
 * @param hdr Header.
 * @return fode.
 */
static struct fode *setup(struct node *n, FILE **fp, int ht, char *hdr) {
	struct fode *f;
	char *name;

	if (*fp) {
		fclose(*fp);
		*fp = NULL;
	}

	name = n->key + strlen(n->key) + 1;
	if ((f = find_file(name)) == NULL) {
		u_error("File '%s' disappeared from list!?!?", name);
	}

	if ((*fp = fopen(f->f_name, "ab+")) == NULL) {
		u_error("Failed to open '%s'", f->f_name);
	}

	if (hdr) {
		if (!(f->flags & ht)) {
			tmp = hdr;
			html_out(*fp, "<H3>@@tmp@@</H3>\n<P CLASS=\"right\">");
			f->flags |= ht;
		}
	}
	
	file = f->b_name;
	if (*n->key) {
		tmp = n->key;
	} else {
		tmp = "Unnamed";
	}

	return f;
}

/**
 * End usage of fp.
 *
 * @param fp Where fp is stored.
 */
static void end_use(FILE **fp) {
	if (*fp) {
		fclose(*fp);
		*fp = NULL;
	}
}

/**
 * Write a subtable (which, in fact, isn't a table at all!)
 *
 * @param fp File pointer.
 * @param src Source data.
 */
static void sub_table(FILE *fp, char *src) {
	char c;

	html_out(fp, "<DIV CLASS=\"npr\">");
	for (EVER) {
		for (tmp = src; (*src) && (*src != '\t'); src++);

		if (tmp == src) {
			break;
		}

		*src = 0;
		tmp = u_trim(tmp);
		html_out(fp, "<CODE>@@tmp@@</CODE>");
		for (tmp = ++src; (*src) && (*src != '\n'); src++);

		c = *src;
		*src = 0;
		tmp = u_trim(tmp);
		html_out(fp, " - @@tmp@@<BR>\n");
		if (!c) {
			break;
		} else {
			src++;
		}
	}
	html_out(fp, "</DIV>\n");
}

/**
 * Output string to all files.
 *
 * @param src Source html string.
 */
static void run_string(char *src) {
	struct fode *f;
	FILE *fp;

	for (f = file_list; f; f = f->next) {
		if ((fp = fopen(f->f_name, "ab+")) == NULL) {
			u_error("Failed to open '%s'", f->f_name);
		}
	
		file = f->b_name;
		html_out(fp, src);
		fclose(fp);
	}
}

/**
 * Write the headers for the files on this list (if not already present).
 *
 * @param n First node.
 */
static void write_headers(struct node *n) {
	struct fode *f;
	char *b;
	FILE *fp;

	while (n) {
		if ((b = strchr(n->key, 1))) {
			*b++ = 0;
			if ((f = find_file(b)) == NULL) {
				f = add_file(b);

				if ((fp = fopen(f->f_name, "wb+")) == NULL) {
					u_error("Failed to open '%s'", f->f_name);
				}
	
				file = f->b_name;
				html_out(fp, html_header);
				fclose(fp);
			}
		}

		n = n->next;
	}
}

/**
 * Write formatted Html output.
 *
 * @param fp File pointer.
 * @param html Html source string.
 */
static void html_out(FILE *fp, char *html) {
	int len;
	char *p;

	while ((p = u_strinstr(html, "@@"))) {
		if ((len = p - html)) {
			if (fwrite(html, len, 1, fp) != 1) {
				u_error(NULL, "Failed to write HTML output");
			}
		}

		if (strncasecmp(p, "@@file@@", 8) == 0) {
			fprintf(fp, "%s", basename(file));
			p += 8;
		} else if (strncasecmp(p, "@@style@@", 9) == 0) {
			fprintf(fp, "%s", style_sheet);
			p+=9;
		} else if (strncasecmp(p, "@@filelist@@", 12) == 0) {
			int pre = 0;
			struct fode *f;

			for (f = file_list; f; f = f->next) {
				if (strcasecmp(file, f->b_name) == 0) {
					fprintf(fp, "%s<A HREF=\"%s\" CLASS=\"selected\">%s</A>\n", (pre ? "<BR>" : ""), f->f_name, basename(f->b_name));
				} else {
					fprintf(fp, "%s<A HREF=\"%s\" CLASS=\"left\">%s</A>\n", (pre ? "<BR>" : ""), f->f_name, basename(f->b_name));
				}

				pre = 1;
			}

			p += 12;
		} else if (strncasecmp(p, "@@tmp@@", 7) == 0) {
			fprintf(fp, "%s", tmp);
			p += 7;
		}

		if (ferror(fp)) {
			u_error("Failed to write HTML output");
		}

		html = p;
	}

	if (*html) {
		fprintf(fp, "%s", html);
	}

	if (ferror(fp)) {
		u_error("Failed to write HTML output");
	}
}

/**
 * Generate index page.
 */
void index_page(void) {
	FILE *fp = NULL;
	struct node *n;
	struct fode *f;
	char *p;

	if ((fp = fopen("index.html", "wb+")) == NULL) {
		u_error("Failed to open index.html");
	}

	file = "Index";
	html_out(fp, html_header);
	html_out(fp, html_left);
	html_out(fp, html_right);

	u_out(XO_DEBUG, "Sorting index list");
	index_nodes = NULL;
	add_to_index(e_list);
	add_to_index(s_list);
	add_to_index(v_list);
	add_to_index(f_list);
	add_to_index(t_list);

	for (n = index_nodes; n; n = n->next) {
		tmp = n->key;
		p = n->key + strlen(n->key) + 1;
		if ((f = find_file(p)) == NULL) {
			u_error("File '%s' disappeared from list!?!?", p);
		}

		file = f->f_name;
		html_out(fp, "<A HREF=\"@@file@@#@@tmp@@\" CLASS=\"index\">@@tmp@@</A><BR>");
	}

	html_out(fp, html_footer);
	fclose(fp);
}

/**
 * Add the list starting with this node to the index list. Entries are sorted.
 *
 * @param n First node.
 */
static void add_to_index(struct node *n) {
	struct node **prev = &index_nodes;
	struct node *i, *l;
	int c, done;

	for (; n; n = n->next) {
		i = u_alloc(sizeof(struct node));
		i->key = n->key;
		i->desc = n->desc;
		i->signature = n->signature;
		i->param = n->param;
		i->ret = n->ret;
		i->note = n->note;

		done = 0;
		while (((l = *prev) != NULL) && (!done)) {
			if ((c = strcasecmp(i->key, l->key)) < 0) {
				i->next = l;
				*prev = i;
				done = 1;
			} else {
				prev = &l->next;
			}
		}

		if (!done) {
			*prev = i;
			i->next = l;
		}
	}
}
