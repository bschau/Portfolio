#define MAIN
#include "include.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: makemap world-output-file\r\n");
		exit(1);
	}

	map_reset();
	map_preclose();
	map_close_cells();
	map_remove_sparks();
	map_terrainify();
	map_create_cities();
	map_find_player_start_position();
	map_dump();
	json_dump(argv[1]);
	exit(0);
}
