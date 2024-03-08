#include "include.h"

/**
 * Delete command.
 */
void cmd_delete(int argc, char *argv[], int optind)
{
	struct zen_node *z, *zp;

	if ((argc - optind) < 1) {
		fprintf(stderr, "delete id1 id2 .. idN\n");
		exit(1);
	}

	zen_file_read();

	for (; optind < argc; optind++) {
		if ((z = zen_node_by_id(atoi(argv[optind]), &zp)) != NULL) {
			if (zp != NULL) {
				zp->next = z->next;
			}

			free(z);
		}
	}

	zen_file_write();
}
