#include "include.h"

/**
 * View command.
 */
void cmd_view(int argc, char *argv[], int optind)
{
	struct zen_node *z;

	if ((argc - optind) != 1) {
		fprintf(stderr, "Usage: view id\n");
		exit(1);
	}

	zen_file_read();

	if ((z = zen_node_by_id(atoi(argv[optind]), NULL)) == NULL) {
		fprintf(stderr, "Story with id %s not found\n", argv[optind]);
	} else {
		printf("%s\n", zen_decode(z->story));
	}
}
