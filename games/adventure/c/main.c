#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EVER (;;)

static void adventure(void);
static int range_random(int limit);
static void get_enter(void);
static void found_potion(void);
static void found_treasure(void);
static void found_trap(void);
static void found_monster(void);
static char fight_or_bribe(int cost);

static int coins = 0, strength = 500;

int main(int argc, char *argv[])
{
	srand(time(NULL));

	printf("Adventure\n");
	printf("Welcome to the dark caves ... prepare to die!\n");
	printf("\n");
	printf("Press the enter key at the prompt to continue ...\n");
	printf("\n");
	adventure();
	printf("\n");
}

static void adventure(void)
{
	int turn = 1;

	for EVER {
		printf("Turn: %d   Coins: %d   Strength: %d\n", turn, coins, strength);
		if (strength < 1) {
			printf("So long - nice knowing you. R.I.P.\n");
			return;
		}

		switch (range_random(4)) {
			case 0:
				found_potion();
				break;
			case 1:
				found_treasure();
				break;
			case 2:
				found_trap();
				break;
			case 3:
				found_monster();
				break;
		}

		printf("> ");
		get_enter();
		turn++;
	}
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

static void found_potion(void)
{
	int energy = range_random(30) + 1;
	printf("You found a potion giving you %d extra strength.\n", energy);
	strength += energy;
}

static void found_treasure(void)
{
	int treasure = range_random(100) + 100;
	printf("Hooray, you found a treasure of %d coins.\n", treasure);
	coins += treasure;
}

static void found_trap(void)
{
	int penalty = range_random(50) + 10;
	printf("Bummer - you sprung a trap. It hurt you %d strength.\n", penalty);
	strength -= penalty;
}

static void found_monster(void)
{
	char *monsters[4] = { "snake", "kobold", "goblin", "troll" };
	int penalties[4] = { 10, 20, 30, 60 };

	int monster = range_random(4);
	char *monster_type = monsters[monster];
	int bribe = range_random(75) + 1;
	if (bribe > coins) {
		printf("A %s snuck up on you and dealt %d damage.\n", monster_type, bribe);
		strength -= bribe;
		return;
	}

	printf("Beware - you met a %s\n", monster_type);

	for EVER {
		char key = fight_or_bribe(bribe);
		if (key == 'f') {
			int damage = penalties[monster];
			printf("You killed the %s but it dealt you %d damage.\n", monster_type, damage);
			strength -= damage;
			if (monster != 0) {
				return;
			}

			if (range_random(100) > 9) {
				return;
			}

			printf("The snake was guarding a treasure - ");
			if (range_random(100) > 3) {
				int worth = range_random(100) + 1;
				printf("%d coins ...\n", worth);
				coins += worth;
				return;
			}

			int worth = (range_random(4) + 1) * 100;
			printf("a huge diamond, worth %d coins\n", worth);
			coins += worth;
			return;
		} else if (key == 'b') {
			printf("... and so you pay %d coins and run away!\n", bribe);
			coins -= bribe;
			return;
		}
	}
}

static char fight_or_bribe(int cost)
{
	int show_prompt = 1;
	char response = ' ';

	for EVER {
		if (show_prompt) {
			printf("Fight or bribe (f / b)?\n");
			printf("(Bribing will cost you %d coins).\n", cost);
			show_prompt = 0;
		}

		int key = getchar();
		if (key == '\n') {
			if (response != ' ') {
				break;
			}

			show_prompt = 1;
		} else if (key == 'F' || key == 'f') {
			response = 'f';
			break;
		} else if (key == 'B' || key == 'b') {
			response = 'b';
			break;
		}
	}

	return response;
}
