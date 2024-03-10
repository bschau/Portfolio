#include "include.h"

static void create_city_name(void);
static void place_city(int x, int y);

static char *_chunk_1[] = {
	"a", "e", "i", "o", "u", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};

static char *_chunk_2[] = {
	"b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "q", "r", "s", "t", "v", "w", "x", "y", "z", "br", "cr", "dr", "fr", "gr", "kr", "pr", "qr", "sr", "tr", "vr", "wr", "yr", "zr", "str", "bl", "cl", "fl", "gl", "kl", "pl", "sl", "vl", "yl", "zl", "ch", "kh", "ph", "sh", "yh", "zh"
};

static char *_chunk_3[] = {
	"a", "e", "i", "o", "u", "ae", "ai", "ao", "au", "aa", "ee", "ea", "ei", "eo", "eu", "ia", "ie", "io", "iu", "oa", "oe", "oi", "oo", "ou", "ua", "ue", "ui", "uo", "uu", "a", "e", "i", "o", "u", "a", "e", "i", "o", "u", "a", "e", "i", "o", "u", "a", "e", "i", "o", "u", "a", "e", "i", "o", "u"
};

static char *_chunk_4[] = {
	"c", "d", "f", "h", "k", "l", "m", "n", "p", "r", "s", "t", "x", "y", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};

static char *_chunk_5[] = {
	"bert" ,"bridge", "burg", "burgh", "burn", "bury", "bus", "by", "caster", "cester", "chester", "dale", "dence", "diff", "ding", "don", "fast", "field", "ford", "gan", "gas", "gate", "gend", "ginia", "gow", "ham", "hull", "land", "las", "ledo", "lens", "ling", "mery", "mond", "mont", "more", "mouth", "nard", "phia", "phis", "polis", "pool", "port", "pus", "ridge", "rith", "ron", "rora", "ross", "rough", "sa", "sall", "sas", "sea", "set", "sey", "shire", "son", "stead", "stin", "ta", "tin", "tol", "ton", "vale", "ver", "ville", "vine", "ving", "well", "wood"
};

static int _dx, _dy, _city_type;
static char city_name[CITY_NAME_LENGTH];

void map_create_cities(void)
{
	struct City *city;
	int x, y;

	for (y = 0; y < MAP_H / QUAD_SIZE; y++) {
		for (x = 0; x < MAP_W / QUAD_SIZE; x++) {
			place_city(x, y);
			create_city_name();

			city = (struct City *)calloc(1, sizeof(struct City));
			if (city == NULL) {
				perror("map_create_cities");
				exit(1);
			}

			city->x = _dx;
			city->y = _dy;
			city->type = _city_type;
			strncpy(city->name, city_name, CITY_NAME_LENGTH);
			city->next = cities;
			cities = city;
		}
	}
}

static void place_city(int x, int y)
{
	for (;;) {
		_dx = (QUAD_SIZE * x) + (rand() % QUAD_SIZE);
		_dy = (QUAD_SIZE * y) + (rand() % QUAD_SIZE);

		int idx = TO_MAP(_dx, _dy);
		if (map[idx] != I_LAKE) {
			if (map[idx] == I_MOUNTAIN) {
				_city_type = CITY_TYPE_DWARF;
			} else if (map[idx] == I_FOREST) {
				_city_type = CITY_TYPE_ELVER;
			} else {
				_city_type = CITY_TYPE_HUMAN;
			}

			map[idx] = I_CITY;
			return;
		}
	}
}

static void create_city_name(void)
{
	int idx1 = rand() % (sizeof(_chunk_1) / sizeof(char *));
	int idx2 = rand() % (sizeof(_chunk_2) / sizeof(char *));
	int idx3 = rand() % (sizeof(_chunk_3) / sizeof(char *));
	int idx4 = rand() % (sizeof(_chunk_4) / sizeof(char *));
	int idx5 = rand() % (sizeof(_chunk_5) / sizeof(char *));

	snprintf(city_name, CITY_NAME_LENGTH, "%s%s%s%s%s",
		_chunk_1[idx1],
		_chunk_2[idx2],
		_chunk_3[idx3],
		_chunk_4[idx4],
		_chunk_5[idx5]);

	city_name[0] = (char)toupper(city_name[0]);
}
