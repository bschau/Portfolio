#include "include.h"

static void dump_map(FILE *fh);
static void dump_cities(FILE *fh);

void json_dump(char *output)
{
	FILE *fh = fopen(output, "w+b");
	if (fh == NULL) {
		perror("json_dump");
		exit(1);
	}

	fprintf(fh, "loadWorld('{");
	fprintf(fh, "\"map_w\": %i,\"map_h\": %i,", MAP_W, MAP_H);
	dump_map(fh);
	fprintf(fh, ",");
	dump_cities(fh);
	fprintf(fh, ", \"player\":{\"x\": %i,\"y\": %i}", ply_x, ply_y);

	fprintf(fh, "}');\r\n");
	fclose(fh);
}

static void dump_map(FILE *fh)
{
	uint8_t *m = map;

	fprintf(fh, "\"map\": [");
	char separator = ' ';
	int y, x;
	for (y = 0; y < MAP_H; y++) {
		for (x = 0; x < MAP_W; x++) {
			fprintf(fh, "%c%i", separator, *m++);
			separator = ',';
		}
	}
	fprintf(fh, "]");
}

static void dump_cities(FILE *fh)
{
	struct City *city;

	fprintf(fh, "\"cities\": [");
	char divider = ' ';

	for (city = cities; city; city = city->next) {
		fprintf(fh, " %c{ \"x\":%i,\"y\":%i,\"type\":%i,\"name\":\"%s\" }", divider, city->x, city->y, city->type, city->name);
		divider = ',';
	}
	fprintf(fh, "]");
}
