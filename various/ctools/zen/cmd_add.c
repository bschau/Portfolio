#include "include.h"

/**
 * Add command.
 */
void cmd_add(int argc, char *argv[], int optind)
{
	zen_file_read();

	if ((argc - optind) > 0) {
		for (; optind < argc; optind++) {
			if (argv[optind][0] == '@') {
				char *mem = argv[optind];

				zen_add_story(read_file(mem + 1));
			} else {
				zen_add_story(argv[optind]);
			}
		}

		zen_file_write();
	} else {
		char *story = editor(NULL);
		zen_add_story(story);
		zen_file_write();
	}
}
