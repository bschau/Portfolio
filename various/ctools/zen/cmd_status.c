#include <time.h>
#include "include.h"

/** forward decls */
static void _dump(struct zen_node *z);

/** globals */
static char pre[2];

/**
 * Status command.
 */
void cmd_status(int argc, char *argv[], int optind)
{
	struct zen_node *z;

	zen_file_read();

	pre[0] = 0;
	pre[1] = 0;

	if ((argc - optind) > 0) {
		for (; optind < argc; optind++) {
			if ((z = zen_node_by_id(atoi(argv[optind]), NULL)) == NULL) {
				fprintf(stderr, "Cannot find story with id %s\n", argv[optind]);
			} else {
				_dump(z);
			}
		}
	} else {
		while ((z = zen_next()) != NULL) {
			_dump(z);
		}

	}
}

/**
 * Dump content of node.
 *
 * @param z Node.
 */
static void _dump(struct zen_node *z)
{
	printf("%sStory ID: %i\n", pre, z->id);
	printf("Created: %s", ctime(&z->created));

	if (z->created != z->modified) {
		printf("Modified: %s", ctime(&z->modified));
	}

	if (z->closed > 0) {
		printf("Closed: %s", ctime(&z->closed));
	}

	pre[0] = '\n';
}
