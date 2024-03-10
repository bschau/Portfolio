#ifndef INCLUDE_H
#define INCLUDE_H

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAP_W (200)
#define MAP_H (200)
#define QUAD_SIZE (40)
#define I_CLEAR (0)
#define I_MOUNTAIN (1)
#define I_LAKE (2)
#define I_FOREST (3)
#define I_CITY (4)
#define CITY_TYPE_HUMAN (1)
#define CITY_TYPE_ELVER (CITY_TYPE_HUMAN + 1)
#define CITY_TYPE_DWARF (CITY_TYPE_ELVER + 1)
#define TO_MAP(x, y) (((y) * MAP_W) + (x))
#define CITY_NAME_LENGTH (32)

struct City {
	struct City *next;
	int x;
	int y;
	int type;
	char name[CITY_NAME_LENGTH + 1];
};

/* json.c */
void json_dump(char *output);

/* map_cities.c */
void map_create_cities(void);

/* map_close.c */
void map_preclose(void);
void map_close_cells(void);

/* map_remove_sparks.c */
void map_remove_sparks(void);

/* map_terrainify.c */
void map_terrainify(void);

/* map_utils.c */
void map_reset();
int map_number_of_neighbours(int x, int y);
void map_dump(void);
void map_find_player_start_position(void);

#ifdef MAIN
uint8_t *map;
struct City *cities;
int ply_x, ply_y;
#else
extern uint8_t *map;
extern struct City *cities;
extern int ply_x, ply_y;
#endif

#endif
