#include "common.h"
#include "include.h"

/* macros */
#define IGNORE_LINE_LEN 1024

/* globals */
static struct ignore_node *list = NULL;

/**
 * Ignore command.
 */
void cmd_ignore(int argc, char *argv[], int optind) {
	if ((argc - optind) < 1) {
		fprintf(stderr, "Missing one or more patterns\n");
		exit(1);
	}

	config_find();

	ignore_list_read();
	for (; optind < argc; optind++) {
		if (ignore_list_find(argv[optind]) == NULL) {
			ignore_list_add(argv[optind]);
		}
	}

	ignore_list_write();
}

/**
 * Read the ignore list.
 */
void ignore_list_read(void) {
	struct stat s_buf;
	FILE *fp;

	ignore_list_destroy();

	if (stat(IGNOREFILE, &s_buf) == 0) {
		char line[IGNORE_LINE_LEN + 1];
		char *s;

		if ((fp = fopen(IGNOREFILE, "rb")) == NULL) {
			perror(IGNOREFILE);
			exit(1);
		}

		while ((s = xgets(line, IGNORE_LINE_LEN, fp))) {
			if (*s) {
				if (ignore_list_find(s) == NULL) {
					ignore_list_add(s);
				}
			}
		}

		fclose(fp);
	}
}

/**
 * Write the ignore list.
 */
void ignore_list_write(void) {
	FILE *fp = fopen(IGNOREFILE, "wb");
	struct ignore_node *n = list;

	if (fp == NULL) {
		perror(IGNOREFILE);
		exit(1);
	}

	while (n) {
		fprintf(fp, "%s\n", IN_ENTRY(n));
		n = n->next;
	}

	fclose(fp);

	set_file_permissions(IGNOREFILE, 0600);
}

/**
 * Destroy the ignore list.
 */
void ignore_list_destroy(void) {
	if (list) {
		struct ignore_node *n = list, *t;

		while (n) {
			t = n->next;
			free(n);
			n = t;
		}

		list = NULL;
	}
}

/**
 * Find an entry on the ignore list.
 *
 * @param e Entry.
 * @return node or NULL.
 */
struct ignore_node *ignore_list_find(char *e) {
	struct ignore_node *n = list;

	while (n) {
		if (strcmp(IN_ENTRY(n), e) == 0) {
			return n;
		}

		n = n->next;
	}

	return NULL;
}

/**
 * Add the entry to the ignore list.
 *
 * @param e Entry to add.
 */
void ignore_list_add(char *e) {
	int e_len = strlen(e);
	struct ignore_node *n = (struct ignore_node *) malloc(IN_LEN + e_len + 1);
	char *p;

	if (n == NULL) {
		fprintf(stderr, "Out of memory adding to the ignore list\n");
		exit(1);
	}

	n->next = list;
	p = IN_ENTRY(n);
	strncpy(p, e, e_len);
	p[e_len] = 0;

	list = n;
}

/**
 * See if any node on the ignore list matches this entry.
 *
 * @param e Entry.
 * @return 0 if no matches, !0 if one or more matches found.
 */
int ignore_list_match(char *e) {
	struct ignore_node *n = list;

	while (n) {
		if (strstr(e, IN_ENTRY(n)) != NULL) {
			return 1;
		}

		n = n->next;
	}

	return 0;
}

/**
 * Print ignore list
 */
void ignore_list_print(char sep) {
	struct ignore_node *n = list;

	while (n) {
		printf("%c%s", sep, IN_ENTRY(n));

		n = n->next;
	}
}
