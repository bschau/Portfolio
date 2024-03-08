#include <time.h>
#include "include.h"

/**
 * Close command.
 */
void cmd_close(int argc, char *argv[], int optind)
{
	time_t now = time(NULL);
	struct zen_node *z;

	zen_file_read();

	for (; optind < argc; optind++) {
		if ((z = zen_node_by_id(atoi(argv[optind]), NULL)) != NULL) {
			z->closed = now;
		}
	}

	zen_file_write();
}
