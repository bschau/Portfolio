#include "include.h"

static int _neighbours = 4, _iterations = 50000;
static int _close_cell_probability = 45, _probability_exceeded = 1;

void map_preclose(void)
{
	int x, y;

	for (y = 0; y < MAP_H; y++) {
		for (x = 0; x < MAP_W; x++) {
			int probability = rand() % 100;
			if (probability < _close_cell_probability) {
				map[TO_MAP(x, y)] = I_MOUNTAIN;
			}
		}
	}
}

void map_close_cells(void)
{
	int true_cell = _probability_exceeded ? I_MOUNTAIN : I_CLEAR;
	int false_cell = true_cell == I_MOUNTAIN ? I_CLEAR : I_MOUNTAIN;
	int i;

	for (i = 0; i < _iterations; i++) {
		int rx = rand() % MAP_W;
		int ry = rand() % MAP_H;
		int neighbours = map_number_of_neighbours(rx, ry);
		map[TO_MAP(rx, ry)] = neighbours > _neighbours ? true_cell : false_cell;
	}
}
