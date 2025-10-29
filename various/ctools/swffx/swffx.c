#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#ifdef Linux
#include <libgen.h>
#endif
#define MAIN
#include "common.h"

/* macros */
#define EVER ;;

/* structs */
struct node {
	struct node *next;
	char *file;
};

/* forward decls */
static void usage(int ec);
static void add_files(int argc, char *argv[]);
static void merge_variables(void);
static void merge(char **dst, char *cmd, char *sc);
static void open_output(void);
static void emit_header(FILE *f);
static void emit_footer(FILE *f);
static void generate_page(void);
static void add_to_list(char *f);
static void script_parse(char *file);
static void read_file(char *file, size_t size);
static void pickup_arg(char *token, char **dst);
static void parse(void);
static void effect_ftb(FILE *f);
static void effect_ota(FILE *f);

/* globals */
static char *d_fps = "25", *d_swf_name = NULL, *d_version = "8", *d_bbox = NULL;
static char *d_output = "swffx.sc", *d_quality = "100%", *d_type = "jpeg";
static char *d_alpha_lo = "0%", *d_alpha_hi = "100%";
static char *d_output_html = "0", *d_fade = "40", *d_pause = "100";
static char *d_run_swfc = "1", *d_effect = "ota";
static char *d_background_image = NULL, *d_offset_x = "0", *d_offset_y = "0";
static char *v_fps = NULL, *v_swf_name = NULL, *v_version = NULL;
static char *v_bbox = NULL, *v_output = NULL, *v_quality = NULL;
static char *v_type = NULL, *v_alpha_lo = NULL, *v_alpha_hi = NULL;
static char *v_output_html = NULL, *v_fade = NULL, *v_pause = NULL;
static char *v_run_swfc = NULL, *v_effect = NULL;
static char *v_background_image = NULL, *v_offset_x = NULL, *v_offset_y = NULL;
static char *l_fps = NULL, *l_swf_name = NULL, *l_version = NULL;
static char *l_bbox = NULL, *l_output = NULL, *l_quality = NULL;
static char *l_type = NULL, *l_alpha_lo = NULL, *l_alpha_hi = NULL;
static char *l_output_html = NULL, *l_fade = NULL, *l_pause = NULL;
static char *l_run_swfc = NULL, *l_effect = NULL;
static char *l_background_image = NULL, *l_offset_x = NULL, *l_offset_y = NULL;
static char *script = NULL, *mem;
static FILE *out = NULL;
static int a_fade, a_pause;
static struct node *list = NULL;
static int list_cnt = 0, line;

/**
 * Write usage information and exit with 'ec' status.
 *
 * @param ec Error code.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "swffx %s\n", bstools_version);
	fprintf(f, "Usage: swffx [OPTIONS] [file1 file2 ... fileN]\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -B image      Background image\n");
	fprintf(f, "  -C            Files on command line before files in script\n");
	fprintf(f, "  -E effect     Effect, ota|ftb (defaults to %s)\n", d_effect);
	fprintf(f, "  -F fps        Frames pr. seconds (defaults to %s)\n", d_fps);
	fprintf(f, "  -N            Don't run swfc\n");
	fprintf(f, "  -P            Generate test HTML page\n");
	fprintf(f, "  -S name       SWF name\n");
	fprintf(f, "  -V version    Flash file version (defaults to %s)\n", d_version);
	fprintf(f, "  -X offset     X offset of smaller pictures (defaults to %s)\n", d_offset_x);
	fprintf(f, "  -Y offset     Y offset of smaller pictures (defaults to %s)\n", d_offset_y);
	fprintf(f, "  -b wxh        Bounding box (wxh)\n");
	fprintf(f, "  -f frames     Frames length for action (defaults to %s)\n", d_fade);
	fprintf(f, "  -h value      High alpha value (defaults to %s)\n", d_alpha_hi);
	fprintf(f, "  -l value      Low alpha value (defaults to %s)\n", d_alpha_lo);
	fprintf(f, "  -o file       Output file (defaults to %s\n", d_output);
	fprintf(f, "  -p frames     Pause frames length (defaults to %s)\n", d_pause);
	fprintf(f, "  -q quality    Quality of loaded graphics files (defaults to %s)\n", d_quality);
	fprintf(f, "  -s file       Script file to read input\n");
	fprintf(f, "  -t type       Type of loaded graphics files, gif|jpeg|png (defaults to %s)\n", d_type);

	exit(ec);
}

/**
 * Main.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 if successful, !0 otherwise.
 */
int main(int argc, char *argv[]) {
	int cmd_first = 0, opt;

	while ((opt = getopt(argc, argv, "B:CE:F:NPS:V:X:Y:b:f:h:l:o:p:q:s:t:")) != -1) {
		switch (opt) {
		case 'B':
			l_background_image = optarg;
			break;

		case 'C':
			cmd_first = 1;
			break;

		case 'E':
			l_effect = optarg;
			break;

		case 'F':
			l_fps = optarg;
			break;

		case 'N':
			l_run_swfc = "0";
			break;

		case 'P':
			l_output_html = optarg;
			break;

		case 'S':
			l_swf_name = optarg;
			break;

		case 'V':
			l_version = optarg;
			break;

		case 'X':
			l_offset_x = optarg;
			break;

		case 'Y':
			l_offset_y = optarg;
			break;

		case 'b':
			l_bbox = optarg;
			break;

		case 'f':
			l_fade = optarg;
			break;

		case 'h':
			l_alpha_hi = optarg;
			break;

		case 'l':
			l_alpha_lo = optarg;
			break;

		case 'o':
			l_output = optarg;
			break;

		case 'p':
			l_pause = optarg;
			break;

		case 'q':
			l_quality = optarg;
			break;

		case 's':
			script = optarg;
			break;

		case 't':
			l_type = optarg;
			break;

		default:
			fprintf(stderr, "Unknown argument '%c'\n", (char) opt);
			usage(1);
		}
	}

	if (script == NULL) {
		if ((argc - optind) < 2) {
			usage(1);
		}

		add_files(argc, argv);
	} else {
		if (cmd_first) {
			add_files(argc, argv);
			script_parse(script);
		} else {
			script_parse(script);
			add_files(argc, argv);
		}
	}

	if (list_cnt < 2) {
		fprintf(stderr, "Only one picture given ...\n");
		exit(1);
	}

	merge_variables();

	a_fade = atoi(d_fade);
	a_pause = atoi(d_pause);

	if (strcasecmp(d_effect, "ota") == 0) {
		open_output();
		effect_ota(out);
	} else if (strcasecmp(d_effect, "ftb") == 0) {
		open_output();
		effect_ftb(out);
	} else {
		fprintf(stderr, "Unknown effect '%s'\n", d_effect);
		exit(1);
	}

	fclose(out);

	if ((opt = atoi(d_run_swfc))) {
		int len = strlen(d_output);
		char *m;
#ifdef WIN32
		len += 10;		/* "swfc.exe "+NUL */
#else
		len += 6;			/* "swfc "+NUL */
#endif
		if ((m = malloc(len)) == NULL) {
			fprintf(stderr, "Out of memory - cannot run swfc\n");
			exit(1);
		}

#ifdef WIN32
		snprintf(m, len, "swfc.exe %s", d_output);
#else
		snprintf(m, len, "swfc %s", d_output);
#endif
		opt = system(m);
	}

	if ((opt = atoi(d_output_html))) {
		generate_page();
	}

	exit(0);
}

/**
 * Add files from command line to files list.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 */
static void add_files(int argc, char *argv[]) {
	for (; optind < argc; optind++) {
		add_to_list(argv[optind]);
	}
}

/**
 * Merge variables.
 */
static void merge_variables(void) {
	merge(&d_fps, l_fps, v_fps);
	merge(&d_swf_name, l_swf_name, v_swf_name);
	merge(&d_version, l_version, v_version);
	merge(&d_bbox, l_bbox, v_bbox);
	merge(&d_output, l_output, v_output);
	merge(&d_quality, l_quality, v_quality);
	merge(&d_type, l_type, v_type);
	merge(&d_alpha_lo, l_alpha_lo, v_alpha_lo);
	merge(&d_alpha_hi, l_alpha_hi, v_alpha_hi);
	merge(&d_output_html, l_output_html, v_output_html);
	merge(&d_fade, l_fade, v_fade);
	merge(&d_pause, l_pause, v_pause);
	merge(&d_run_swfc, l_run_swfc, v_run_swfc);
	merge(&d_effect, l_effect, v_effect);
	merge(&d_background_image, l_background_image, v_background_image);
	merge(&d_offset_x, l_offset_x, v_offset_x);
	merge(&d_offset_y, l_offset_y, v_offset_y);
}

/**
 * Merge variables.
 *
 * @param dst default (= destination variable).
 * @param cmd command line variables.
 * @param sc script variables.
 */
static void merge(char **dst, char *cmd, char *sc) {
	if (cmd) {
		*dst = cmd;
	} else if (sc) {
		*dst = sc;
	}
}

/**
 * Open output file.
 */
static void open_output(void) {
	if ((out = fopen(d_output, "wb+")) == NULL) {
		fprintf(stderr, "%s:%s\n", d_output, strerror(errno));
		exit(1);
	}
}

/**
 * Write swfc header.
 *
 * @param f output file.
 */
static void emit_header(FILE *f) {
	struct node *n = list;
	int i;

	fprintf(f, ".flash");
	if (d_swf_name) {
		fprintf(f, " name=\"%s\"", d_swf_name);
	}

	if (d_bbox) {
		fprintf(f, " bbox=%s", d_bbox);
	}

	if (d_version) {
		fprintf(f, " version=%s", d_version);
	}

	if (d_fps) {
		fprintf(f, " fps=%s", d_fps);
	}

	fprintf(f, "\n");

	for (i = 0; i < list_cnt; i++) {
		fprintf(f, ".%s s%i \"%s\"", d_type, i, n->file);
		if (strcasecmp(d_type, "jpeg") == 0) {
			if (d_quality) {
				fprintf(f, " quality=%s", d_quality);
			}
		}

		fprintf(f, "\n");
		n = n->next;
	}

	if (d_background_image) {
		fprintf(f, ".%s b \"%s\"", d_type, d_background_image);
		if (strcasecmp(d_type, "jpeg") == 0) {
			if (d_quality) {
				fprintf(f, " quality=%s", d_quality);
			}
		}
		fprintf(f, "\n");
		fprintf(f, ".put b x=0 y=0 alpha=100%%\n");
	}
}

/**
 * Write swfc footer.
 *
 * @param f output file.
 */
void emit_footer(FILE *f) {
	fprintf(f, ".end\n");
}

/**
 * Generate html page.
 */
static void generate_page(void) {
	char *p = strrchr(d_output, (int)'.');
	FILE *f = NULL;
	int i;

	if (p == NULL) {
		i = strlen(d_output) + 6;
		if ((p = malloc(i)) == NULL) {
			fprintf(stderr, "Cannot generate output HTML page\n");
			exit(1);
		}

		snprintf(p, i, "%s.html", d_output);
	} else {
		i = (p - d_output);
		if ((p = malloc(i + 6)) == NULL) {
			fprintf(stderr, "Cannot generate output HTML page\n");
			exit(1);
		}
		memmove(p, d_output, i);
		memmove(p + i, ".html", 5);
		*(p + i + 5) = 0;
	}

	if ((f = fopen(p, "wb+")) == NULL) {
		fprintf(stderr, "Failed to open HTML page (%s)\n", p);
		exit(1);
	}

	fprintf(f, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
	fprintf(f, "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
	fprintf(f, "  <head>\n    <meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-1\" />\n    <title>swffx</title>\n  </head>\n");
	fprintf(f, "  <body>\n    <p><object>\n");
	fprintf(f, "      <param name=\"movie\" value=\"%s\" />\n", d_swf_name);
	fprintf(f, "      <param name=\"loop\" value=\"true\" />\n");
	fprintf(f, "      <param name=\"quality\" value=\"high\" />\n");

	if (d_bbox) {
		int l = strlen(d_bbox);

		for (i = 0; i < l; i++) {
			if (d_bbox[i] == 'x') {
				d_bbox[i] = 0;
				fprintf(f, "      <embed src=\"%s\" width=\"%i\" height=\"%i\" />\n", d_swf_name, atoi(d_bbox), atoi(d_bbox + i + 1));
				d_bbox[i] = 'x';
				break;
			}
		}
	} else {
		fprintf(f, "      <embed src=\"%s\" />\n", d_swf_name);
	}

	fprintf(f, "    </object></p>\n  </body>\n</html>\n");
	fclose(f);
}

/**
 * Add node to the end of the list.
 *
 * @param f File.
 */
static void add_to_list(char *f) {
	struct node **prev = &list;
	struct node *l = malloc(sizeof(struct node) + strlen(f) + 1), *n;

	if (l == NULL) {
		fprintf(stderr, "Cannot allocate new node\n");
		exit(1);
	}

	l->next = NULL;
	l->file = ((char *)l) + sizeof(struct node);
	memmove(l->file, f, strlen(f) + 1);

	while ((n = *prev) != NULL) {
		prev = &n->next;
	}

	*prev = l;
	list_cnt++;
}


/**
 * Read and parse a script file.
 *
 * @param file Script file to read.
 */
static void script_parse(char *file) {
	struct stat s_buf;

	if (stat(file, &s_buf) != 0) {
		fprintf(stderr, "Failed to stat %s\n", file);
		exit(1);
	}

	if ((mem = malloc(s_buf.st_size + 2)) == NULL) {
		fprintf(stderr, "Cannot allocate %i bytes\n", (int) s_buf.st_size + 2);
		exit(1);
	}

	read_file(file, s_buf.st_size);
	parse();
}

/**
 * Read the file into memory.
 *
 * @param file File name.
 * @param size Size of file.
 */
static void read_file(char *file, size_t size) {
	FILE *fp = fopen(file, "rb");

	if (fp == NULL) {
		fprintf(stderr, "Failed to open %s\n", file);
		exit(1);
	}

	if (fread(mem, size, 1, fp) != 1) {
		fprintf(stderr, "Failed to read %s\n", file);
		exit(1);
	}
	
	mem[size] = 0;
	mem[size + 1] = 0;
	fclose(fp);
}

/**
 * Parse the script file.
 */
static void parse(void) {
	char *token;

	line = 1;
	for (EVER) {
		for (; *mem && isspace(*mem); mem++) {
			if (*mem == '\n') {
				line++;
			}
		}

		if (!*mem) {
			break;
		}

		if (*mem == '#') {
			for (; *mem; mem++) {
				if (*mem == '\n') {
					line++;
					break;
				}
			}
			continue;
		}

		token = mem;
		for (; *mem && !isspace(*mem); mem++) {
			if (*mem == '\n') {
				break;
			}
		}

		*mem++ = 0;

		if (strcasecmp(token, "fps") == 0) {
			pickup_arg(token, &v_fps);
		} else if (strcasecmp(token, "swfname") == 0) {
			pickup_arg(token, &v_swf_name);
		} else if (strcasecmp(token, "version") == 0) {
			pickup_arg(token, &v_version);
		} else if (strcasecmp(token, "bbox") == 0) {
			pickup_arg(token, &v_bbox);
		} else if (strcasecmp(token, "output") == 0) {
			pickup_arg(token, &v_output);
		} else if (strcasecmp(token, "quality") == 0) {
			pickup_arg(token, &v_quality);
		} else if (strcasecmp(token, "type") == 0) {
			pickup_arg(token, &v_type);
		} else if (strcasecmp(token, "lo") == 0) {
			pickup_arg(token, &v_alpha_lo);
		} else if (strcasecmp(token, "hi") == 0) {
			pickup_arg(token, &v_alpha_hi);
		} else if (strcasecmp(token, "generatehtml") == 0) {
			v_output_html = "1";
		} else if (strcasecmp(token, "fadeframes") == 0) {
			pickup_arg(token, &v_fade);
		} else if (strcasecmp(token, "pauseframes") == 0) {
			pickup_arg(token, &v_pause);
		} else if (strcasecmp(token, "dontrunswfc") == 0) {
			v_run_swfc = "0";
		} else if (strcasecmp(token, "effect") == 0) {
			pickup_arg(token, &v_effect);
		} else if (strcasecmp(token, "picture") == 0) {
			char *f = NULL;

			pickup_arg(token, &f);
			add_to_list(f);
		} else if (strcasecmp(token, "backgroundimage") == 0) {
			pickup_arg(token, &v_background_image);
		} else if (strcasecmp(token, "offsetx") == 0) {
			pickup_arg(token, &v_offset_x);
		} else if (strcasecmp(token, "offsety") == 0) {
			pickup_arg(token, &v_offset_y);
		} else {
			fprintf(stderr, "%i:Unknown token '%s'\n", line, token);
			exit(1);
		}
	}
}

/**
 * Get argument.
 *
 * @param token token.
 * @param dst where to store argument.
 */
void pickup_arg(char *token, char **dst) {
	char c = 0;

	for (; *mem && isspace(*mem); mem++) {
		if (*mem == '\n') {
			fprintf(stderr, "%i:no value to token '%s'\n", line, token);
			exit(1);
		}
	}

	switch (*mem) {
		case '\'':
		case '"':
			c = *mem++;
			break;
	}
	*dst = mem;

	for (EVER) {
		if (*mem == '\n') {
			if (c) {
				fprintf(stderr, "%i:unterminated string value to token '%s'\n", line, token);
				exit(1);
			}
			*mem++ = 0;
			break;
		}

		if ((c) && (*mem == c)) {
			*mem++ = 0;
			break;
		} else if (isspace((int) *mem)) {
			*mem++ = 0;
			break;
		}

		mem++;
	}
}

/*******************************************************************************
 *
 * Effects.
 *
 */

/**
 * Create script for "Fade to Black" effect.
 *
 * @param f output file.
 */
static void effect_ftb(FILE *f) {
	int i, frame, x;

	emit_header(f);

	frame = a_fade;
	fprintf(f, ".put s0 x=%s y=%s alpha=%s\n", d_offset_x, d_offset_y, d_alpha_lo);
	for (i = 0; i < list_cnt; i++) {
		x = ((i + 1) == list_cnt ? 0 : i + 1);

		fprintf(f, ".frame %i\n", frame);
		fprintf(f, ".change s%i alpha=%s\n", i, d_alpha_hi);
		frame += a_pause;
		fprintf(f, ".frame %i\n", frame);
		fprintf(f, ".change s%i alpha=%s\n", i, d_alpha_hi);
		frame += a_fade;
		fprintf(f, ".frame %i\n", frame);
		fprintf(f, ".change s%i alpha=%s\n", i, d_alpha_lo);
		fprintf(f, ".put s%i x=%s y=%s alpha=%s\n", x, d_offset_x, d_offset_y, d_alpha_lo);
		fprintf(f, ".del s%i\n", i);
		frame += a_fade;
	}
	fprintf(f, ".action:\n\tgotoFrame(0);\n\tPlay();\n.end\n");

	emit_footer(f);
}

/**
 * Create script for "One to Another" effect.
 *
 * @param f output file.
 */
static void effect_ota(FILE *f) {
	int i, frame, x;

	emit_header(f);

	frame = a_fade;
	fprintf(f, ".put s%i\n", list_cnt - 1);
	fprintf(f, ".del s%i\n", list_cnt - 1);
	fprintf(f, ".put s0 x=%s y=%s alpha=%s\n", d_offset_x, d_offset_y, d_alpha_lo);
	fprintf(f, ".frame %i\n", frame);
	fprintf(f, ".change s0 alpha=%s\n", d_alpha_hi);
	fprintf(f, ".frame %i\n", (frame + 1));
	fprintf(f, ".put s1 x=%s y=%s alpha=%s\n", d_offset_x, d_offset_y, d_alpha_lo);

	for (i = 1; i < list_cnt; i++) {
		x = ((i + 1) == list_cnt ? 0 : i + 1);

		frame += a_pause;
		fprintf(f, ".frame %i\n", frame);
		fprintf(f, ".change s%i alpha=%s\n", (i - 1), d_alpha_hi);
		fprintf(f, ".change s%i alpha=%s\n", i, d_alpha_lo);
		frame += a_fade;
		fprintf(f, ".frame %i\n", frame);
		fprintf(f, ".change s%i alpha=%s\n", (i - 1), d_alpha_lo);
		fprintf(f, ".change s%i alpha=%s\n", i, d_alpha_hi);
		fprintf(f, ".frame %i\n", (frame + 1));
		fprintf(f, ".del s%i\n", (i - 1));
		fprintf(f, ".put s%i x=%s y=%s alpha=%s\n", x, d_offset_x, d_offset_y, d_alpha_lo);
	}

	frame += a_pause;
	fprintf(f, ".frame %i\n", frame);
	fprintf(f, ".change s%i alpha=%s\n", (list_cnt - 1), d_alpha_hi);
	fprintf(f, ".change s0 alpha=%s\n", d_alpha_lo);

	frame += a_fade;
	fprintf(f, ".frame %i\n", frame);
	fprintf(f, ".change s%i alpha=%s\n", (list_cnt - 1), d_alpha_lo);
	fprintf(f, ".change s0 alpha=%s\n", d_alpha_hi);
	fprintf(f, ".action:\n\tgotoFrame(%i);\n\tPlay();\n.end\n", a_fade + 1);

	emit_footer(f);
}
