/**
 * This is the main entry class. Usage of gencdoc:
 *
 * <PRE>
 * gencdoc [OPTIONS] files<BR>
 * <BR>
 * [OPTIONS]<BR>
 *     -S style      Style sheet<BR>
 *     -h            This page<BR>
 *     -o dir        Output files to 'dir'<BR>
 *     -s            Add static declarations to output<BR>
 *     -v            Raise verbosity<BR>
 * </PRE>
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#define MAIN
#include "common.h"
#include "include.h"

/** forward decls */
static void usage(int ec);
static void dump_list(char *t, struct node *n);

/**
 * Write usage information and exit with 'ec' status.
 *
 * @param ec Error code.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stdout : stderr;

	fprintf(f, "gencdoc %s\n", bstools_version);
	fprintf(f, "Usage: gencdoc [OPTIONS] files\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -S style      Style sheet\n");
	fprintf(f, "  -h            This page\n");
	fprintf(f, "  -o dir        Output files to 'dir'\n");
	fprintf(f, "  -s            Add static declarations to output\n");
	fprintf(f, "  -v            Raise verbosity\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	char *o_dir = "doc", *style = NULL;
	struct stat s_buf;
	int opt;

	while ((opt = getopt(argc, argv, "S:ho:sv")) != -1) {
		switch (opt) {
		case 'S':
			style = optarg;
			break;

		case 'h':
			usage(0);

		case 'o':
			o_dir = optarg;
			break;

		case 's':
			add_statics = 1;
			break;

		case 'v':
			if (verbose < XO_DEBUG) {
				verbose++;
			}
			break;

		default:
			fprintf(stderr, "Unknown option: '%c'\n", (char) opt);
			usage(1);
		}
	}

	if ((argc - optind) < 1) {
		usage(1);
	}

	if (style) {
		FILE *fp;

		u_out(XO_DEBUG, "Loading external style sheet '%s'", style);

		if (stat(style, &s_buf)) {
			u_error("Failed to stat '%s'", style);
		}

		style_sheet = u_alloc(s_buf.st_size + 2);
		if ((fp = fopen(style, "rb")) == NULL) {
			u_error("Failed to open '%s'", style);
		}

		if (fread(style_sheet, s_buf.st_size, 1, fp) != 1) {
			u_error("Failed to read '%s'", style);
		}

		fclose(fp);
		style_sheet = u_trim(style_sheet);
	}

	parse_open();
	u_out(XO_INFO, "Loading files");
	for (; optind < argc; optind++) {
		parse_new(argv[optind]);
		parse();
		parse_free();
	}
	
	if ((opt = stat(o_dir, &s_buf))) {
		if (errno != ENOENT) {
			u_error("Failed to stat '%s' (%i)", o_dir, errno);
		} else {
#ifndef WIN32
			if (mkdir(o_dir, 0777)) {
#else
			if (mkdir(o_dir)) {
#endif
				u_error("Failed to create output directory '%s'", o_dir);
			}
		}
	} else {
		if (!(S_ISDIR(s_buf.st_mode))) {
			u_error("%s exists but is not a directory", o_dir);
		}
	}

	if (chdir(o_dir)) {
		u_error("Failed to change directory to '%s'", o_dir);
	}

	u_out(XO_INFO, "Generating pages");
//	dump_list("Functions", f_list);
	output();

	u_out(XO_INFO, "Generating index");
	index_page();

	parse_close();

	if (style) {
		free(style_sheet);
	}

	u_out(XO_INFO, "Done");
	exit(0);
}

/**-
 * Dump list beginning with this node.
 *
 * @param t Title.
 * @param n Node.
 */
static void dump_list(char *t, struct node *n) {
	printf("%s ====>", t);

	if (n) {
		printf("\n");
		for (; n; n = n->next) {
			printf("    %s\n", n->key);
		}
	} else {
		printf(" none\n");
	}
}
