#include "include.h"

void map_remove_sparks(void)
{
	int x, y;

	for (y = 0; y < MAP_H; y++) {
		for (x = 0; x < MAP_W; x++) {
			int idx = TO_MAP(x, y);
			if (map[idx] == I_CLEAR) {
				continue;
			}

			if (map_number_of_neighbours(x, y) != 1) {
				continue;
			}

			map[TO_MAP(x, y)] = I_CLEAR;
		}
	}
}
