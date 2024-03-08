#include "include.h"

/** forward decls */
static void _exiter(void);

/** macros */
#define TEMPLATE "zenXXXXXX"

/** globals */
static char *template;
static int fd = -1;

/**
 * atexit handler.
 */
static void _exiter(void)
{
	if (template != NULL) {
		if (fd != -1) {
			close(fd);
			fd = -1;
		}

		if (strcmp(template, TEMPLATE) != 0) {
			if (unlink(template) != 0) {
				perror("unlink");
			}
		}

		template = NULL;
	}
}

/**
 * Spawn editor and return response (if any).
 *
 * @param story Story text or NULL.
 * @return Encoded response.
 */
char *editor(char *story)
{
	StringBuilder s_bld = stringbuilder_new(512);
	char *ed = getenv("ZEN_EDITOR");

	if (ed == NULL) {
		ed = getenv("EDITOR");
	}

	if (ed == NULL) {
		fprintf(stderr, "Please define ZEN_EDITOR/EDITOR to point to your editor\n");
		exit(1);
	}

	if (atexit(_exiter)) {
		perror("atexit");
		exit(1);
	}

	if ((template = strdup(TEMPLATE)) == NULL) {
		perror("strdup");
		exit(1);
	}

	if ((fd = mkstemp(template)) < 0) {
		perror("mkstemp");
		exit(1);
	}

	if (story) {
		if (write(fd, story, strlen(story)) == -1) {
			perror("write");
			exit(1);
		}
	}

	close(fd);
	fd = -1;

	stringbuilder_append(s_bld, ed, strlen(ed));
	stringbuilder_append(s_bld, " ", 1);
	char *args = getenv("ZEN_EDITOR_ARGS");
	if (args != NULL) {
		stringbuilder_append(s_bld, args, strlen(args));
		stringbuilder_append(s_bld, " ", 1);
	}
	stringbuilder_append(s_bld, template, strlen(template));

	if (system(stringbuilder_to_string(s_bld)) != 0) {
		perror("system");
		exit(1);
	}

	story = read_file(template);
	if (unlink(template) == 0) {
		template = NULL;
	} else {
		perror("unlink");
	}

	return story;
}
