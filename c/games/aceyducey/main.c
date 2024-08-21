#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EVER (;;)

#define CLUBS (0)
#define SPADES (CLUBS + 1)
#define HEARTS (SPADES + 1)
#define DIAMONDS (HEARTS + 1)
#define INITIAL_MONEY (100)

static void acey_ducey(void);
static int range_random(int limit);
static void init_cards(void);
static void init_suite(int suite);
static void set_card_name(unsigned char c);
static int get_bet(void);
static int did_player_win(unsigned char card_1, unsigned char card_2, unsigned char player);

static char *card_names[] = { "unused", "unused", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace" };
static char *card_suites[] = { "clubs", "spades", "hearts", "diamonds" };

static unsigned char cards[52];
static char card_name[32];
static int money = INITIAL_MONEY;

int main(int argc, char *argv[])
{
	srand(time(NULL));

	printf("Acey Ducy\n=========\n\n");
	printf("Two cards are drawn and shown face up.\n");
	printf("You bet on whether the third card drawn falls between the two cards.\n");
	printf("Enter 0 as amount to skip betting.\n");
	printf("If you win your bet is doubled as a price.\n");
	printf("The game will stop when there are no more cards to draw\n");
	printf("or when you run out of money.\n");
	printf("\n");
	acey_ducey();
	printf("\n");
	exit(0);
}

static int range_random(int limit)
{
	return rand() % limit;
}

static void acey_ducey(void)
{
	init_cards();
	int index = 0;
	int round = 0;

	for EVER {
		if (money <= 0) {
			break;
		}

		int cards_left = sizeof(cards) - index;
		if (cards_left < 3) {
			break;
		}

		round++;
		printf("Round %i\n", round);
		printf("You have $%i\n", money);

		printf("The two cards:\n");
		int card_1 = cards[index++];
		int card_2 = cards[index++];
		set_card_name(card_1);
		printf("\t%s\n", card_name);
		set_card_name(card_2);
		printf("\t%s\n", card_name);
		int bet = get_bet();
		if (bet == 0) {
			continue;
		}

		money -= bet;
		int player = cards[index++];
		printf("Your card:\n");
		set_card_name(player);
		printf("\t%s\n\n", card_name);

		if (did_player_win(card_1, card_2, player)) {
			printf("You won $%i!\n", bet);
			money += bet + bet;
		} else {
			printf("You lost $%i. Better luck next round!\n", bet);
		}

		printf("\n");
	}

	printf("\nGame over.\n");
	if (money > INITIAL_MONEY) {
		printf("You came out with $%i more than you entered with!!\n", money - INITIAL_MONEY);
	} else if (money == INITIAL_MONEY) {
		printf("You win some, you lose some. Nothing gained or lost.\n");
	} else {
		printf("Bummer! You lost $%i.\n", INITIAL_MONEY - money);
	}
}

static void init_cards()
{
	init_suite(CLUBS);
	init_suite(SPADES);
	init_suite(HEARTS);
	init_suite(DIAMONDS);

	printf("Shuffling cards ...\n\n");
	for (int i = 0; i < 1000; i++) {
		int l = range_random(sizeof(cards));
		int r = range_random(sizeof(cards));
		if (l == r) {
			continue;
		}

		unsigned char t = cards[l];
		cards[l] = cards[r];
		cards[r] = t;
	}
}

static void init_suite(int suite)
{
	int index = suite * 13;
	suite <<= 4;

	for (int c = 2; c < 15; c++) {
		cards[index++] = c | suite;
	}
}

static void set_card_name(unsigned char c)
{
	snprintf(card_name, sizeof(card_name), "%s of %s", card_names[c & 0xf], card_suites[c >> 4]);
}

static int get_bet(void)
{
	int bet = 0;

	for EVER {
		printf("\nHow much du you want to bet?\n> ");
		int ret = scanf("%d", &bet);
		if (ret != 1 || bet < 0) {
			continue;
		}

		if (bet == 0) {
			printf("\nChicken!\n\n");
			break;
		}

		if (bet > money) {
			printf("\nYou don't have that amount of money to bet.\n\n");
			continue;
		}

		break;
	}

	return bet;
}

static int did_player_win(unsigned char card_1, unsigned char card_2, unsigned char player)
{
	card_1 &= 0xf;
	card_2 &= 0xf;
	player &= 0xf;

	if (card_1 > card_2) {
		unsigned char c = card_1;
		card_1 = card_2;
		card_2 = c;
	}

	return player > card_1 && player < card_2;
}
