#include "include.h"

void map_reset()
{
	map = calloc(MAP_W * MAP_H, sizeof(uint8_t));
	if (map == NULL) {
		perror("map_reset");
		exit(1);
	}

	time_t now = time(NULL);
	srand(now);
}

int map_number_of_neighbours(int x, int y)
{
	int count = 0, dx, dy;

	for (dy = -1; dy < 2; dy++) {
		for (dx = -1; dx < 2; dx++) {
			int rx = x + dx;
			int ry = y + dy;
			if (rx < 0 || rx >= MAP_W || ry < 0 || ry >= MAP_H) {
				continue;
			}

			if (map[TO_MAP(rx, ry)] == I_MOUNTAIN) {
				count++;
			}
		}
	}

	return count;
}

void map_dump(void)
{
	FILE *fh = fopen("map.pnm", "w+b");
	uint8_t *m = map;

	if (fh == NULL) {
		perror("map_dump");
		exit(1);
	}

	fprintf(fh, "P3\n%i %i\n255\n", MAP_W, MAP_H);

	int x, y;
	for (y = 0; y < MAP_H; y++) {
		for (x = 0; x < MAP_W; x++) {
			if (*m == I_CLEAR) {
				fprintf(fh, "255 255 255\n");
			} else if (*m == I_MOUNTAIN) {
				fprintf(fh, "205 170 125\n");
			} else if (*m == I_LAKE) {
				fprintf(fh, "100 149 237\n");
			} else if (*m == I_CITY) {
				fprintf(fh, "0 0 0\n");
			} else {
				fprintf(fh, "34 139 34\n");
			}
			
			m++;
		}
	}

	fclose(fh);
}

void map_find_player_start_position(void)
{
	int x = MAP_W / 2;
	int y = MAP_H / 2;
	int stride = 1, count = 1;
	int direction = 0;

	for (;;) {
		if (map[TO_MAP(x, y)] == I_CLEAR) {
			ply_x = x;
			ply_y = y;
			return;
		}

		count--;
		if (count == 0) {
			stride++;
			count = stride - 1;
			direction++;
			direction = direction & 3;
		}

		if (direction == 1) {
			x++;
		} else if (direction == 2) {
			y++;
		} else if (direction == 3) {
			x--;
		} else {
			y--;
		}
	}
}
