#include "include.h"

void _find_size(int x, int y);
void _paint(int x, int y, uint8_t color);

static uint8_t *_backmap;
static int _size;

void map_terrainify(void)
{
	_backmap = calloc(MAP_W * MAP_H, sizeof(uint8_t));
	if (_backmap == NULL) {
		perror("map_terrainify");
		exit(1);
	}
	
	int x, y;
	for (y = 0; y < MAP_H; y++) {
		for (x = 0; x < MAP_W; x++) {
			int idx = TO_MAP(x, y);
			if (_backmap[idx]) {
				continue;
			}

			if (map[idx] != I_MOUNTAIN) {
				continue;
			}

			_size = 0;
			_find_size(x, y);
			if (_size > 80) {
				continue;
			}

			int pct = rand() % 100;
			if (pct < 25) {
				_paint(x, y, I_LAKE);
			} else if (pct < 80) {
				_paint(x, y, I_FOREST);
			}
		}
	}
}

void _find_size(int x, int y)
{
	if (x < 0 || x > (MAP_W - 1) || y < 0 || y > (MAP_H - 1)) {
		return;
	}

	int idx = TO_MAP(x, y);

	if (_backmap[idx]) {
		return;
	}

	if (map[idx] != I_MOUNTAIN) {
		return;
	}

	_backmap[idx] = 1;
	_size++;

	_find_size(x - 1, y - 1);
	_find_size(x, y - 1);
	_find_size(x + 1, y - 1);
	_find_size(x - 1, y);
	_find_size(x + 1, y);
	_find_size(x - 1, y + 1);
	_find_size(x, y + 1);
	_find_size(x + 1, y + 1);
}

void _paint(int x, int y, uint8_t color)
{
	if (x < 0 || x > (MAP_W - 1) || y < 0 || y > (MAP_H - 1)) {
		return;
	}

	int idx = TO_MAP(x, y);

	if (_backmap[idx] > 1) {
		return;
	}

	if (map[idx] != I_MOUNTAIN) {
		return;
	}

	_backmap[idx] = 2;
	map[idx] = color;

	_paint(x - 1, y - 1, color);
	_paint(x, y - 1, color);
	_paint(x + 1, y - 1, color);
	_paint(x - 1, y, color);
	_paint(x + 1, y, color);
	_paint(x - 1, y + 1, color);
	_paint(x, y + 1, color);
	_paint(x + 1, y + 1, color);
}
