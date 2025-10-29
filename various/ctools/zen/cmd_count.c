#include <time.h>
#include "include.h"

/**
 * Count command.
 */
void cmd_count(int argc, char *argv[], int optind, int count0)
{
	int only_open = 1;
	struct zen_node *z;

	while (optind < argc) {
		if (strcasecmp(argv[optind], "all") == 0 ||
			strcasecmp(argv[optind], "a") == 0) {

			only_open = 0;
		} else {
			fprintf(stderr, "Unknown option to list: %s\n", argv[optind]);
			exit(1);
		}

		optind++;
	}

	zen_file_read();

	int count = 0;
	while ((z = zen_next()) != NULL) {
		if ((only_open) && (z->closed > 0)) {
			continue;
		}

		count++;
	}

	printf("%li", (long int)count);
	if (!count0) {
		printf("\n");
	}
}
