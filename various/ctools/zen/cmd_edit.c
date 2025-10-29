#include "include.h"

/**
 * Edit command.
 */
void cmd_edit(int argc, char *argv[], int optind)
{
	char *mem = "";
	struct zen_node *node;
	int id;

	zen_file_read();

	if ((argc - optind) < 1) {
		fprintf(stderr, "edit id [\"story\" | @story]\n");
		exit(1);
	}

	id = atoi(argv[optind++]);
	if ((node = zen_node_by_id(id, NULL)) == NULL) {
		fprintf(stderr, "No such story\n");
		exit(1);
	}

	if ((argc - optind) >= 1) {
		if (argv[optind][0] == '@') {
			mem = argv[optind];

			mem = read_file(mem + 1);
		} else {
			mem = argv[optind];
		}
	} else {
		mem = editor(zen_decode(node->story));
	}

	mem = zen_trim_story(mem);
	if ((mem == NULL) || (strlen(mem) < 1)) {
		fprintf(stderr, "Empty story\n");
		exit(1);
	}

	node->story = zen_encode(mem);
	zen_file_write();
}
