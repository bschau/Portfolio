#include "include.h"

/**
 * Reopen command.
 */
void cmd_reopen(int argc, char *argv[], int optind)
{
	struct zen_node *z;

	zen_file_read();

	for (; optind < argc; optind++) {
		if ((z = zen_node_by_id(atoi(argv[optind]), NULL)) != NULL) {
			z->closed = 0;
		}
	}

	zen_file_write();
}
