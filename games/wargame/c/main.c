#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "wargame.h"

static void wargame(void);
static int range_random(int limit);
static void get_enter(void);
static void init_army(struct army *army);
static void show_army(struct army *army, char *label);
static char get_order(void);
static int handle_order(struct army *attacker, struct army *defender, char order, char *att_name, char *def_name);
static void battle(int *attacker, int *defender, char *title, char *att_name, char *def_name);
static void handle_response(void);
static void adjust_production(void);
static int get_production(int production);

static struct army west, east;

int main(int argc, char *argv[])
{
	srand(time(NULL));

	printf("Wargame\n=======\n\n");
	printf("West vs East ... you play west, I play east.\n");
	printf("Prepare to die!\n\n");
	wargame();
	printf("\n");
	exit(0);
}

static int range_random(int limit)
{
	return rand() % limit;
}

static void get_enter(void)
{
	for EVER {
		int key = getchar();
		if (key == '\n') {
			break;
		}
	}
}

static void wargame(void)
{
	init_army(&east);
	init_army(&west);

	int day = 1;
	for EVER {
		printf("Conflict, day %d\n", day);
		show_army(&west, "You");
		show_army(&east, "Me");
		printf("\nAwaiting your orders, General!\n\n");
		printf("  %s Bomb airfields  %s Bomb harbour            %s Wage war at sea\n", west.airplanes > 0 ? "1." : "  ", west.airplanes > 0 ? "2." : "  ", west.ships > 0 ? "3." : "  ");
		printf("  %s Invade East     %s Bomb aircraft factory   %s Bomb harbour dock\n", west.soldiers > 0 ? "4." : "  ", west.airplanes > 0 ? "5." : "  ", west.airplanes > 0 ? "6." : "  ");
		printf("  7. Do nothing\n\n");
		char order = get_order();

		if (!handle_order(&west, &east, order, "You", "east")) {
			printf("\n\nCannot do that ...\n\n");
			get_enter();
			continue;
		}

		handle_response();
		adjust_production();

		day++;
		if (day > 25) {
			break;
		}

		printf("\n\n\nPress enter to continue ...\n\n\n");
		get_enter();
	}

	int total_west = west.airplanes + west.ships + west.soldiers;
	int total_east = east.airplanes + east.ships + east.soldiers;

	if (total_west < total_east) {
		printf("\n\n E A S T wins this battle .. Sorry General, you're demoted to soldier!\n");
	} else if (total_west > total_east) {
		printf("\n\nWay to go, General! You won!\n");
		printf("And in the later years you retire, write a book about it and become\nfilthy rich. But - hey! You earned it! :-)\n");
	} else {
		printf("\n\nPeace!\n");
	}
}

static void init_army(struct army *army)
{
	army->airplanes = 100;
	army->ships = 100;
	army->soldiers = 1000;
	army->airplanes_production = 9;
	army->ships_production = 9;
}

static void show_army(struct army *army, char *label)
{
	printf("%s:\n", label);
	printf("  Airplanes: %d (%d), ships: %d (%d), soldiers: %d\n",
		army->airplanes, army->airplanes_production,
		army->ships, army->ships_production, army->soldiers);
}

static char get_order(void)
{
	char response = ' ';

	printf("> ");
	for EVER {
		int key = getchar();
		if (key == '\n') {
			if (response != ' ') {
				break;
			}
			printf("> ");
		} else if (key >= '1' && key <= '7') {
			response = key;
		} else {
			response = ' ';
		}
	}

	printf("\n");
	return response;
}

static int handle_order(struct army *attacker, struct army *defender, char order, char *att_name, char *def_name)
{
	switch (order) {
		case '1':
			if (attacker->airplanes < 1) {
				return 0;
			}

			battle(&attacker->airplanes, &defender->airplanes, "%s bombed %s airfields, dealing %i damage while losing %i airplane%s.", att_name, def_name);
			break;
		case '2':
			if (attacker->airplanes < 1) {
				return 0;
			}

			battle(&attacker->airplanes, &defender->ships, "%s boombed %s harbour, dealing %i damage while losing %i airplane%s.", att_name, def_name);
			break;
		case '3':
			if (attacker->ships < 1) {
				return 0;
			}

			battle(&attacker->ships, &defender->ships, "%s battled %s at sea, dealing %i damage while losing %i ship%s.", att_name, def_name);
			break;
		case '4':
			if (attacker->soldiers < 1) {
				return 0;
			}

			battle(&attacker->soldiers, &defender->soldiers, "%s invaded %s, dealing %i damage while losing %i soldier%s.", att_name, def_name);
			break;
		case '5':
			if (attacker->airplanes < 1) {
				return 0;
			}

			battle(&attacker->airplanes, &defender->airplanes_production, "%s bombed %s aircraft factories, dealing %i damage while losing %i airplane%s.", att_name, def_name);
			break;
		case '6':
			if (attacker->airplanes < 1) {
				return 0;
			}

			battle(&attacker->airplanes, &defender->ships_production, "%s bombed %s harbour docks, dealing %i damage while losing %i airplane%s.", att_name, def_name);
			break;
		case '7':
			printf("Nothing was done ...\n\n");
			break;
	}

	return 1;
}

static void battle(int *attacker, int *defender, char *text, char *att_name, char *def_name)
{
	int att_damage, def_damage;

	if (*defender < 1) {
		return;
	}

	att_damage = range_random(*defender) / 9;
	*attacker -= att_damage;
	if (*attacker < 0) {
		*attacker = 0;
	}

	def_damage = range_random(*attacker) / 10;
	*defender -= def_damage;
	if (*defender < 0) {
		*defender = 0;
	}

	printf(text, att_name, def_name, def_damage, att_damage, att_damage == 1 ? "" : "s");
	printf("\n\n");
}

static void handle_response()
{
	char choices[] = "      ";
	int index = 0;
	if (east.airplanes > 0)
	{
		if (west.airplanes > 0) {
			choices[index++] = '1';
		}

		if (west.ships > 0) {
			choices[index++] = '2';
		}

		if (west.airplanes_production > 0) {
			choices[index++] = '5';
		}

		if (west.ships_production > 0) {
			choices[index++] = '6';
		}
	}

	if (east.ships > 0 && west.ships > 0) {
		choices[index++] = '3';
	}

	if (east.soldiers > 0 && west.soldiers > 0) {
		choices[index++] = '4';
	}

	if (index == 0) {
		printf("East is idle.\n");
		return;
	}

	int choice = choices[range_random(index)];
	(void) handle_order(&east, &west, choice, "East", "your");
}

static void adjust_production()
{
	east.airplanes += get_production(east.airplanes_production);
	east.ships += get_production(east.ships_production);
	west.airplanes += get_production(west.airplanes_production);
	west.ships += get_production(west.ships_production);
}

static int get_production(int production)
{
	if (range_random(50) < 20) {
		return production / 2;
	}

	return production;
}
