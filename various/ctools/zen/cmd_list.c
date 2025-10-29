#include <time.h>
#include "include.h"

/** forward decls */
static void _show_brief_format(int only_open, int width);

/**
 * List command.
 */
void cmd_list(int argc, char *argv[], int optind)
{
	int only_open = 1, brief = 0, width = 80;
	char pre[2];
	struct zen_node *z;

	while (optind < argc) {
		if (strcasecmp(argv[optind], "all") == 0 ||
			strcasecmp(argv[optind], "a") == 0) {
			only_open = 0;
		} else if (strcasecmp(argv[optind], "brief") == 0 ||
			strcasecmp(argv[optind], "b") == 0) {
			brief = 1;
		} else if (strcmp(argv[optind], "-w") == 0) {
			if (optind + 1 >= argc) {
				fprintf(stderr, "You must supply width to '-w' argument\n");
				exit(1);
			}

			optind++;
			width = atoi(argv[optind]);
		} else {
			fprintf(stderr, "Unknown option to list: %s\n", argv[optind]);
			exit(1);
		}

		optind++;
	}

	zen_file_read();

	if (brief) {
		_show_brief_format(only_open, width);
		return;
	}

	pre[0] = 0;
	pre[1] = 0;
	while ((z = zen_next()) != NULL) {
		if ((only_open) && (z->closed > 0)) {
			continue;
		}

		char *time_buffer = strdup(ctime(&z->created)), *p;
		if (time_buffer == NULL) {
			perror("strdup");
			exit(1);
		}

		for (p = time_buffer; *p; p++) {
			if (*p == '\n') {
				*p = 0;
				break;
			}
		}

		printf("%sStory ID: %i\t(%s)\n", pre, z->id, time_buffer);
		if (z->created != z->modified) {
			printf("Modified: %s", ctime(&z->modified));
		}

		if (z->closed > 0) {
			printf("Closed: %s", ctime(&z->closed));
		}

		printf("%s", zen_decode(z->story));
		pre[0] = '\n';
		free(time_buffer);
	}
}

static void _show_brief_format(int only_open, int width)
{
	char *buffer = (char *)malloc(width);
	struct zen_node *z;

	if (buffer == NULL) {
		perror("malloc");
		exit(1);
	}

	while ((z = zen_next()) != NULL) {
		if ((only_open) && (z->closed > 0)) {
			continue;
		}

		char *story = zen_decode(z->story), *p;
		for (p = story; *p; p++) {
			if (*p == '\n') {
				*p = 0;
				break;
			}
		}

		snprintf(buffer, width, "#%d %s", z->id, story);
		if (strlen(buffer) == 79) {
			buffer[78] = '.';
			buffer[77] = '.';
			buffer[76] = '.';
		}
		printf("%s\n", buffer);
	}
	free(buffer);
}
