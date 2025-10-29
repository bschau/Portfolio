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
 * <I>gencdoc</I> tries to auto guess the type being describe (function, structs,
 * typedefs, enums or variables). If <I>gencdoc</I> parses the type incorrectly,
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "common.h"
#include "include.h"

/* macros */
enum { M_DESC = 0, M_RETURN, M_NOTE, M_PARAM_P, M_PARAM_DESC, M_TYPE };
enum { SIG_UNDEF = 0, SIG_NO_SIG, SIG_FUNC, SIG_VAR, SIG_STRUCT, SIG_ENUM, SIG_TYPEDEF };

/* protos */
static void free_list(struct node *ptr);
static void extract(char *src);
static int remode(char *t, int len, int *mode);
static char *skip_white(char *s);
static char *parse_object(char *s);
static int get_sig_type(void);
static void spacer(StringBuilder s, int cnt);
static void function_get(char **name, char **signature);
static void var_get(char **name, char **signature);
static void struct_get(char **name, char **signature);
static void enum_get(char **name, char **signature);
static void typedef_get(char **name, char **signature);
static struct node *build_node(char *key, char *sig);
static void add_node(struct node *n);

/* globals */
static StringBuilder sig_builder, desc_builder, param_builder, note_builder;
static StringBuilder return_builder, type_builder;
static char *mem;
static int sig_type;

/**
 * Initialize parser.
 */
void parse_open(void) {
	u_out(XO_DEBUG, "Allocating parser resources");
	sig_builder = stringbuilder_new(4096);
	desc_builder = stringbuilder_new(4096);
	param_builder = stringbuilder_new(1024);
	note_builder = stringbuilder_new(1024);
	return_builder = stringbuilder_new(1024);
	type_builder = stringbuilder_new(1024);
}

/**
 * Destroy parser.
 */
void parse_close(void) {
	u_out(XO_DEBUG, "Deallocating parser");
	free_list(e_list);
	free_list(f_list);
	free_list(g_list);
	free_list(s_list);
	free_list(t_list);
	free_list(v_list);

	stringbuilder_free(type_builder);
	stringbuilder_free(return_builder);
	stringbuilder_free(note_builder);
	stringbuilder_free(param_builder);
	stringbuilder_free(desc_builder);
	stringbuilder_free(sig_builder);
}

/**
 * Free a list given by ptr.
 *
 * @param ptr First node (or NULL).
 */
static void free_list(struct node *ptr) {
	struct node *t;

	while (ptr) {
		t = ptr->next;
		free(ptr);
		ptr = t;
	}
}

/**
 * Setup for new file parse.
 *
 * @param name File name.
 */
void parse_new(char *name) {
	struct stat s_buf;
	FILE *fp;
	
	u_out(XO_DEBUG, "Loading '%s'", name);
	current_file = NULL;
	if (stat(name, &s_buf)) {
		u_error("Failed to stat '%s'", name);
	}

	mem = u_alloc(s_buf.st_size + 8);

	if ((fp = fopen(name, "rb")) == NULL) {
		u_error("Failed to open '%s'", name);
	}

	if (fread(mem, s_buf.st_size, 1, fp) != 1) {
		u_error("Failed to read '%s'", name);
	}

	fclose(fp);
	current_file = name;
	line_no = 1;
}

/**
 * Close down file parsing for this file.
 */
void parse_free(void) {
	if (mem) {
		free(mem);
		mem = NULL;
	}

	current_file = NULL;
}

/**
 * Parse a file.
 */
void parse(void) {
	char *s, *cmt, *name, *signature;
	int cmt_start;
	struct node *node;

	u_out(XO_DEBUG, "Parsing '%s'", current_file);
	for (s = mem; *s; s++) {
		if (*s == '\n') {
			line_no++;
			continue;
		}

		if ((*s == '/') && (*(s + 1) == '*') && (*(s + 2) == '*')) {
			u_out(XO_DEBUG, "  Comment found on line %i", line_no);
			s += 3;
			if (*s == '-') {
				u_out(XO_DEBUG, "   Ignoring");
				continue;
			}

			cmt_start = line_no;
			for (cmt = s; *s; s++) {
				if ((*s == '*') && (*(s + 1) == '/')) {
					break;
				}

				if (*s == '\n') {
					line_no++;
				}
			}

			if (!*s) {
				u_error(NULL, "Unterminated comment starting on line %i", cmt_start);
			}

			*s = 0;
			s += 2;

			sig_type=SIG_UNDEF;
			name = NULL;
			signature = NULL;
			stringbuilder_reset(desc_builder);
			stringbuilder_reset(param_builder);
			stringbuilder_reset(note_builder);
			stringbuilder_reset(return_builder);
			stringbuilder_reset(sig_builder);
			stringbuilder_reset(type_builder);

			u_out(XO_DEBUG, "  Signature found on line %i", line_no);
			extract(cmt);

			if ((s = parse_object(s)) != NULL) {
				if (sig_type == SIG_UNDEF) {
					sig_type = get_sig_type();
				}

				u_out(XO_DEBUG, "  Signature: (%i) %s", sig_type, stringbuilder_to_string(type_builder));

				if (sig_type > SIG_NO_SIG) {
					switch (sig_type) {
					case SIG_FUNC:
						function_get(&name, &signature);
						break;

					case SIG_VAR:
						var_get(&name, &signature);
						break;

					case SIG_STRUCT:
						struct_get(&name, &signature);
						break;

					case SIG_ENUM:
						enum_get(&name, &signature);
						break;

					case SIG_TYPEDEF:
						typedef_get(&name, &signature);
						break;

					default:
						u_error(NULL, "Unknown signature type");
					}

					if (!add_statics) {
						if (strncmp(signature, "static", 6) == 0) {
							if (signature) {
								free(signature);
							}

							free(name);
							continue;
						}
					}

					if (!name) {
						name = u_strdup(" ");
					}
				} else {
					name = u_strdup(" ");
					signature = NULL;
				}
				s--;

				node = build_node(name, signature);
				add_node(node);

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
static void extract(char *src) {
	int mode = M_DESC, p_pre = 0, len;
	char save;
	char *t;

	for (EVER) {
		for (; (*src) && (isspace((int) *src)); src++);
		if (!*src) {
			break;
		}

		if (*src == '*') {
			src++;
			continue;
		}

		for (t = src; (*src) && (!isspace((int) *src)); src++);
		save = *src;
		*src = 0;
		len = src - t;

		switch (mode) {
		case M_RETURN:
			if (!remode(t, len, &mode)) {
				stringbuilder_append(return_builder, t, len);
				stringbuilder_append_char(return_builder, ' ');
			}
			break;

		case M_NOTE:
			if (!remode(t, len, &mode)) {
				stringbuilder_append(note_builder, t, len);
				stringbuilder_append_char(note_builder, ' ');
			}
			break;

		case M_PARAM_P:
			if (!remode(t, len, &mode)) {
				if (p_pre) {
					stringbuilder_append_char(param_builder, '\n');
				} else {
					p_pre = 1;
				}

				stringbuilder_append(param_builder, t, len);
				stringbuilder_append_char(param_builder, '\t');
				mode = M_PARAM_DESC;
			}
			break;

		case M_PARAM_DESC:
			if (!remode(t, len, &mode)) {
				stringbuilder_append(param_builder, t, len);
				stringbuilder_append_char(param_builder, ' ');
			}
			break;

		case M_TYPE:
			if (!remode(t, len, &mode)) {
				if (strncasecmp(t, "struct", 6) == 0) {
					sig_type = SIG_STRUCT;
				} else if (strncasecmp(t, "enum", 4) == 0) {
					sig_type = SIG_ENUM;
				} else if (strncasecmp(t, "var", 3) == 0) {
					sig_type = SIG_VAR;
				} else if (strncasecmp(t, "func", 4) == 0) {
					sig_type = SIG_FUNC;
				} else if (strncasecmp(t, "typedef", 7) == 0) {
					sig_type = SIG_TYPEDEF;
				} else {
					u_error("Unknown signature type: '%s'", t);
				}
			}
			break;

		default:
			if (!remode(t, len, &mode)) {
				stringbuilder_append(desc_builder, t, len);
				stringbuilder_append_char(desc_builder, ' ');
			}
			break;
		}

		*src = save;
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
static int remode(char *t, int len, int *mode) {
	int m = *mode;

	if (strncasecmp(t, "@param", len) == 0) {
		m = M_PARAM_P;
	} else if (strncasecmp(t, "@member", len) == 0) {
		m = M_PARAM_P;
	} else if (strncasecmp(t, "@return", len) == 0) {
		m = M_RETURN;
	} else if (strncasecmp(t, "@note", len) == 0) {
		m = M_NOTE;
	} else if (strncasecmp(t, "@type", len) == 0) {
		m = M_TYPE;
	} else {
		return 0;
	}

	if (*mode == m) {
		return 0;
	}

	*mode = m;
	return 1;
}

/**
 * Skip whitespaces, update line counter.
 *
 * @param s String.
 * @return Updated pointer.
 */
static char *skip_white(char *s) {
	while (*s) {
		if (*s > ' ') {
			return s;
		}

		if (*s == '\n') {
			line_no++;
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
static char *parse_object(char *s) {
	int level = 0;

	if ((s = skip_white(s)) == NULL) {
		return NULL;
	}

	for (; *s; s++) {
		if (*s == '\n') {
			line_no++;
			*s = ' ';
		} else if ((*s == '/') && (*(s + 1) == '*') && (*(s + 2) == '*')) {
			break;
		} else if (*s == '{') {
			level++;
		} else if (*s == '}') {
			level--;
		} else if ((*s == ';') && (!level)) {
			stringbuilder_append_char(sig_builder, ';');
			break;
		} else if (*s < ' ') {
			continue;
		}

		stringbuilder_append_char(sig_builder, *s);
	}

	*(s - 1) = 0;

	return s;
}

/**
 * Parse the signature.
 */
static int get_sig_type(void) {
	int level = 0;
	char c = -1, term;
	char *sig, *p;

	stringbuilder_append_char(type_builder, ' ');
	for (sig = stringbuilder_to_string(sig_builder); *sig; sig++) {
		if ((*sig == '\'') || (*sig == '"')) {
			term = *sig;
			for (sig++; (*sig) && (*sig != term); sig++);
			if (*sig) {
				sig++;
			}
		} else if ((isspace((int) *sig)) && (!level)) {
			if (c != ' ') {
				stringbuilder_append_char(type_builder, ' ');
				c = ' ';
			}
		} else if (*sig == '{') {
			level++;
			c = '{';
		} else if (*sig == '}') {
			level--;
			c = '}';
		} else {
			if (!level) {
				stringbuilder_append_char(type_builder, *sig);
				c = *sig;
			}
		}
	}

	sig = stringbuilder_to_string(type_builder);
	if (*(sig + 1) == '#') {
		return SIG_NO_SIG;
	}

	if ((p = strchr(sig, '#'))) {
		*p = 0;
	}

	if (u_strinstr(sig, " typedef ")) {
		return SIG_TYPEDEF;
	}

	if (strchr(sig, '(')) {
		return SIG_FUNC;
	}

	if (u_strinstr(sig, " enum ")) {
		return SIG_ENUM;
	}

	if (u_strinstr(sig, " struct ")) {
		return SIG_STRUCT;
	}

	return SIG_VAR;
}

/**
 * Emit X spaces.
 *
 * @param cnt Number of spaces to emit.
 */
static void spacer(StringBuilder s, int cnt) {
	cnt *= 8;
	for (; cnt > 0; cnt--) {
		stringbuilder_append_char(s, ' ');
	}
}

/**
 * Get function signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void function_get(char **name, char **signature) {
	char *sig = u_trim(stringbuilder_to_string(type_builder)), *p;
	int level = 0;
	char last = ' ';

	stringbuilder_reset(sig_builder);
	for (p = sig; *p; p++) {
		if (*p == '(') {
			if (last != ' ') {
				stringbuilder_append_char(sig_builder, ' ');
			}

			level++;
			stringbuilder_append(sig_builder, "(<BR>", 5);
			spacer(sig_builder, level);
			last = ' ';
		} else if (*p == ')') {
			stringbuilder_append(sig_builder, "<BR>)", 5);
			level--;
			if (!level) {
				break;
			}
			last = ')';
		} else if (isspace((int) *p)) {
			if (last != ' ') {
				stringbuilder_append_char(sig_builder, ' ');
				last = ' ';
			}
		} else if (*p == ',') {
			stringbuilder_append(sig_builder, ",<BR>", 5);
			spacer(sig_builder, level);
			last = ' ';
		} else {
			stringbuilder_append_char(sig_builder, *p);
			last = *p;
		}
	}

	sig = u_trim(stringbuilder_to_string(sig_builder));
	*signature = u_strdup(sig);
	p = stringbuilder_to_string(sig_builder) + stringbuilder_length(sig_builder);
	for (--p; p > sig; p--) {
		if (*p == ')') {
			level++;
		} else if (*p == '(') {
			level--;
			if (!level) {
				break;
			}
		}
	}

	for (--p; (p > sig) && (isspace((int) *p)); p--);
	*(p + 1) = 0;
	for (; (p > sig) && (!isspace((int) *p)) && (*p != '*'); p--);
	*name = u_strdup(u_trim(++p));
}

/**
 * Get variable signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void var_get(char **name, char **signature) {
	char *sig = u_trim(stringbuilder_to_string(sig_builder)), *p;
	char last=' ';

	stringbuilder_reset(type_builder);
	for (p = sig; *p; p++) {
		if (isspace((int) *p)) {
			if (last != ' ') {
				last = ' ';
				stringbuilder_append_char(type_builder, ' ');
			}
		} else {
			if (*p == ';') {
				break;
			}

			stringbuilder_append_char(type_builder, *p);
			last = *p;
		}
	}

	sig = u_trim(stringbuilder_to_string(type_builder));
	*signature = u_strdup(sig);

	if ((p = strchr(sig, '[')) == NULL) {
		if ((p = strchr(sig, ';')) == NULL) {
			p = sig + stringbuilder_length(type_builder) - 1;
		}
	} else {
		*p = 0;
		p--;
	}

	for (; (p > sig) && (isspace((int) *p)); p--);

	for (; (p > sig) && (!isspace((int) *p)); p--);
	p++;
	if ((sig = strchr(p, '='))) {
		*sig = 0;
	}

	*name = u_strdup(u_trim(p));
}

/**
 * Get structure signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void struct_get(char **name, char **signature) {
	char *sig = u_trim(stringbuilder_to_string(sig_builder)), *p, *t;
	int level = 0;
	char last = ' ';

	stringbuilder_reset(type_builder);
	for (p = sig; *p; p++) {
		if (isspace((int) *p)) {
			if (last != ' ') {
				last = ' ';
				stringbuilder_append_char(type_builder, ' ');
			}
		} else if (*p == '{') {
			if (last != ' ') {
				stringbuilder_append_char(type_builder, ' ');
			}

			stringbuilder_append(type_builder, "{<BR>", 5);
			level++;
			spacer(type_builder, level);
			last = ' ';
		} else if (*p == '}') {
			stringbuilder_append(type_builder, "}<BR>", 5);
			level--;
			if (!level) {
				break;
			}
			last = '}';
		} else if (*p == ';') {
			stringbuilder_append(type_builder, ";<BR>", 5);
			t = p;
			while (isspace((int) *(t + 1))) {
				t++;
			}

			if (*(t + 1) != '}') {
				spacer(type_builder, level);
			}

			last = ' ';
		} else {
			stringbuilder_append_char(type_builder, *p);
			last = *p;
		}
	}

	sig = u_trim(stringbuilder_to_string(type_builder));
	*signature = u_strdup(sig);

	for (p = sig; (*p) && (*p != '{'); p++);
	for (--p; (p > sig) && (isspace((int) *p)); p--);
	*(p + 1) = 0;
	for (; (p > sig) && (!isspace((int) *p)); p--);
	*name = u_strdup(p + 1);
}

/**
 * Get enum signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void enum_get(char **name, char **signature) {
	char *sig = u_trim(stringbuilder_to_string(sig_builder)), *p, *t;
	int level = 0;
	char last = ' ';

	stringbuilder_reset(type_builder);
	for (p = sig; *p; p++) {
		if (isspace((int) *p)) {
			if (last != ' ') {
				last = ' ';
				stringbuilder_append_char(type_builder, ' ');
			}
		} else if (*p == '{') {
			if (last != ' ') {
				stringbuilder_append_char(type_builder, ' ');
			}

			stringbuilder_append(type_builder, "{<BR>", 5);
			level++;
			spacer(type_builder, level);
			last = ' ';
		} else if (*p == '}') {
			t = p;
			while (isspace((int) *(t - 1))) {
				t--;
			}

			if (*t == ',') {
				stringbuilder_append(type_builder, "}<BR>", 5);
			} else {
				stringbuilder_append(type_builder, "<BR>}<BR>", 9);
			}

			level--;
			if (!level) {
				break;
			}

			last = '}';
		} else if (*p == ',') {
			stringbuilder_append(type_builder, ",<BR>", 5);
			t = p;
			while (isspace((int) *(t + 1))) {
				t++;
			}

			if (*(t + 1) != '}') {
				spacer(type_builder, level);
			}

			last = ' ';
		} else {
			stringbuilder_append_char(type_builder, *p);
			last = *p;
		}
	}

	sig = u_trim(stringbuilder_to_string(type_builder));
	*signature = u_strdup(sig);

	p = u_strinstr(sig, "enum");
	for (p += 4; (*p) && (isspace((int) *p)); p++);
	if (*p != '{') {
		sig = p;
		for (sig = p; (*p) && (!isspace((int) *p)); p++);
		*p = 0;
		*name = u_strdup(sig);
	} else {
		*name = NULL;
	}
}

/**
 * Get typedef signature.
 *
 * @param name Where to store name.
 * @param sig Where to store signature.
 */
static void typedef_get(char **name, char **signature) {
	char *sig = u_trim(stringbuilder_to_string(sig_builder)), *p, *t;
	int level = 0;
	char last = ' ';

	stringbuilder_reset(type_builder);
	for (p = sig; *p; p++) {
		if (isspace((int) *p)) {
			if (last != ' ') {
				last = ' ';
				stringbuilder_append_char(type_builder, ' ');
			}
		} else if (*p == '{') {
			if (last != ' ') {
				stringbuilder_append_char(type_builder, ' ');
			}

			stringbuilder_append(type_builder, "{<BR>", 5);
			level++;
			spacer(type_builder, level);
			last = ' ';
		} else if (*p == '}') {
			last = '}';
			level--;
			if (!level) {
				stringbuilder_append_char(type_builder, '}');
				p++;
				break;
			} else {
				stringbuilder_append(type_builder, "}<BR>", 5);
			}
		} else if (*p == ';') {
			if (level) {
				stringbuilder_append(type_builder, ";<BR>", 5);
				t = p;
				while (isspace((int) *(t + 1))) {
					t++;
				}

				if (*(t + 1) != '}') {
					spacer(type_builder, level);
				}

				last = ' ';
			} else {
				break;
			}
		} else if (*p == ',') {
			stringbuilder_append(type_builder, ",<BR>", 5);
			t = p;
			while (isspace((int) *(t + 1))) {
				t++;
			}

			if (*(t + 1) != '}') {
				spacer(type_builder, level);
			}

			last = ' ';
		} else {
			stringbuilder_append_char(type_builder, *p);
			last = *p;
		}
	}

	for (; (*p) && (*p != ';'); p++) {
		if (isspace((int) *p)) {
			if (last != ' ') {
				stringbuilder_append_char(type_builder, ' ');
				last = ' ';
			}
		} else {
			stringbuilder_append_char(type_builder, *p);
			last = *p;
		}
	}

	sig = u_trim(stringbuilder_to_string(type_builder));
	*signature = u_strdup(sig);

	for (p = sig; *p; p++);
	--p;
	if (*p == ';') {
		--p;
	}

	for (; (p > sig) && (isspace((int) *p)); p--);
	*(p + 1) = 0;
	for (; (p > sig) && (!isspace((int) *p)); p--);
	*name=u_strdup(p + 1);
}

/**
 * Build a new node.
 *
 * @param key Key.
 * @param sig Function signature (or NULL).
 * @return Node.
 */
static struct node *build_node(char *key, char *sig) {
	int size = sizeof(struct node), sig_len=0, f_len, k_len, d_len, p_len, n_len, r_len;
	char *dp, *pp, *np, *rp, *p;
	struct node *n;

	f_len = strlen(current_file) + 1;
	size += f_len;

	if (sig) {
		sig_len = strlen(sig);
		size += sig_len + 1;
	}

	key = u_trim(key);
	k_len = strlen(key);
	size += k_len + 1;

	dp = u_trim(stringbuilder_to_string(desc_builder));
	if ((d_len = strlen(dp))) {
		d_len++;
		size += d_len;
	}

	pp = u_trim(stringbuilder_to_string(param_builder));
	if ((p_len = strlen(pp))) {
		p_len++;
		size += p_len;
	}

	np = u_trim(stringbuilder_to_string(note_builder));
	if ((n_len = strlen(np))) {
		n_len++;
		size += n_len;
	}

	rp = u_trim(stringbuilder_to_string(return_builder));
	if ((r_len = strlen(rp))) {
		r_len++;
		size += r_len;
	}

	n = u_alloc(size);

	p = ((char *) n) + sizeof(struct node);
	n->key = p;
	memmove(p, key, k_len);
	p += k_len;
	*p++ = 1;
	memmove(p, current_file, f_len);
	p += f_len;

	if (d_len) {
		n->desc = p;
		memmove(p, dp, d_len);
		p += d_len;
	}

	if (p_len) {
		n->param = p;
		memmove(p, pp, p_len);
		p += p_len;
	}

	if (r_len) {
		n->ret = p;
		memmove(p, rp, r_len);
		p += r_len;
	}

	if (n_len) {
		n->note = p;
		memmove(p, np, n_len);
		p += n_len;
	}

	if (sig_len) {
		n->signature = p;
		memmove(p, sig, sig_len);
	}

	return n;
}

/**
 * Insert node alphabetically.
 *
 * @param n Node.
 */
static void add_node(struct node *n) {
	struct node **prev;
	struct node *l;
	int c;

	switch (sig_type) {
	case SIG_FUNC:
		prev = &f_list;
		break;

	case SIG_VAR:
		prev = &v_list;
		break;

	case SIG_STRUCT:
		prev = &s_list;
		break;

	case SIG_ENUM:
		prev = &e_list;
		break;

	case SIG_TYPEDEF:
		prev = &t_list;
		break;

	default:
		prev = &g_list;
		break;
	}
		
	while ((l = *prev) != NULL) {
		if ((c = strcasecmp(n->key, l->key)) < 0) {
			n->next = l;
			*prev = n;
			return;
		}

		if (!c) {
			while ((l = *prev) != NULL) {
				int r = strcasecmp(n->key, l->key);

				if (r < 0) {
					prev = &l->next;
				} else if (r == 0) {
					*prev = n;
					n->next = l->next;
					return;
				} else {
					break;
				}
			}

			*prev = n;
			n->next = l;
			return;
		}

		prev = &l->next;
	}

	*prev = n;
	n->next = l;
}
