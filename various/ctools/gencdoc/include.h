#ifndef INCLUDE_H
#define INCLUDE_H

/* macros */
#define EVER ;;
enum { XO_SILENT = 0, XO_INFO, XO_DEBUG };

/* structs */
struct node {
	struct node *next;
	char *key;
	char *desc;
	char *signature;
	char *param;
	char *ret;
	char *note;
};

/* output.c */
void output(void);
void index_page(void);

/* parse.c */
void parse_open(void);
void parse_close(void);
void parse_new(char *name);
void parse_free(void);
void parse(void);

/* utils.c */
void *u_alloc(int cnt);
char *u_strinstr(char *haystack, char *needle);
char *u_strdup(char *src);
char *u_trim(char *src);
void u_error(const char *fmt, ...);
void u_out(int lvl, const char *fmt, ...);
#ifdef WIN32
char *basename(char *path);
#endif

/* globals */
#ifdef MAIN
	char *style_sheet="body { font-family: Verdana, Arial, serif } \
table { border-spacing: 10px; } \
h1 {} \
td.left { vertical-align: top; } \
a.left { font-color: red; } \
td.right {} \
p.global {} \
p.entry { font-size: 1.1em; font-weight: bold; } \
div.signature { margin-left: 2cm; } \
p.desc { margin-left: 2cm; } \
p.right { font-weight: bold; margin-left: 2cm; } \
.npr { margin-left: 4cm; margin-top: -1em; } \
h3 { border: solid 1px; width: 100%; background-color: #87cefa; color: black; } \
code {} \
a.index {} \
a.selected { font-size: 1.1em; border: 1px solid black; }";
	char *current_file;
	int verbose = XO_SILENT, line_no, add_statics;
	struct node *g_list, *e_list, *s_list, *v_list, *f_list, *t_list;
#else
	extern char *style_sheet, *current_file;
	extern int verbose, line_no, add_statics;
	extern struct node *g_list, *e_list, *s_list, *v_list, *f_list, *t_list;
#endif

#endif
