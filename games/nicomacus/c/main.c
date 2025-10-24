#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EVER (;;)

static void nicomacus(void);
static int get_remainder(char *prompt);
static int calculate_number(int rem3, int rem5, int rem7);

int main(int argc, char *argv[])
{
	srand(time(NULL));

	printf("Nicomacus\n");
	nicomacus();
	printf("\n");
}

static void nicomacus(void)
{
	for EVER {
		printf("\n");
		printf("Think of a number between 1 and 100.\n");
		printf("Press CTRL + C to stop.\n");
		int rem3 = get_remainder("Your number divided by 3 has a remainder of");
		int rem5 = get_remainder("Your number divided by 5 has a remainder of");
		int rem7 = get_remainder("Your number divided by 7 has a remainder of");

		int result = calculate_number(rem3, rem5, rem7);
		printf("\nYour number was %d ...\n", result);
	}
}

static int get_remainder(char *prompt)
{
	int remainder = 0;

	printf("\n%s:\n", prompt);

	while (!scanf("%d", &remainder));
	return remainder;
}

static int calculate_number(int rem3, int rem5, int rem7)
{
	int d = (70 * rem3) + (21 * rem5) + (15 * rem7);

	while (d > 105) {
		d -= 105;
	}

	return d;
}
