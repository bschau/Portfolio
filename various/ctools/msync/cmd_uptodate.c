#include "common.h"
#include "include.h"

/**
 * Mark tree as being up to date.
 */
void cmd_uptodate(void) {
	config_find();
	config_write_stamp();
}
