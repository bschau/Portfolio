#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EVER (;;)

static void engine(void);
static int throw_dice(void);
static void get_key(void);

int main(int argc, char *argv[])
{
	srand(time(NULL));

	printf("Craps\n");
	printf("If you throw a 7 or 11 on your first throw, you win immediately. A score\n");
	printf("of 2, 3 or 12 on your first throw means you lose. If you throw anything\n");
	printf("else you get another throw.\n");
	printf("On subsequent throws, you win by matching your first throw, or lose by\n");
	printf("throwing a 7.\n");
	printf("\n");
	printf("Press the enter key at the prompt '>' to continue ...");
	printf("\n");
	engine();
	printf("\n");
}

static void engine(void)
{
	int round = 0, previous = 0, sum;

	for EVER {
		sum = throw_dice() + throw_dice();
		round++;
		if (round == 1) {
			if (sum == 7 || sum == 11) {
				printf("You score %d and win!\n", sum);
				return;
			}

			if (sum == 2 || sum == 3 || sum == 12) {
				printf("You score %d and lose :-(\n", sum);
				return;
			}

			previous = sum;
		} else if (sum == previous) {
			printf("You score %d and win by matching first throw :-)\n", sum);
			return;
		} else if (sum == 7) {
			printf("You score 7 but not in first round, so you lose :-(\n");
			return;
		}

		printf("You score %d\n> ", sum);
		get_key();
	}
}

static int throw_dice(void)
{
	return (rand() % 6) + 1;
}

static void get_key(void)
{
	for EVER {
		int key = getchar();
		if (key == '\n') {
			break;
		}
	}
}
