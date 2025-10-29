#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EVER (;;)

static char yes_no(char *);
static void init(int);
static void draw_board(void);
static void set_cell(char, char *, char *, char *, int);
static void make_move(void);
static int find_hole(char);
static void find_winner(void);
static void get_user_input(void);

static int preferred[9] = { 4, 8, 6, 2, 0, 7, 5, 3, 1 };
static unsigned short move_matrix[8][3] = {
	{ 8, 7, 6 }, { 8, 5, 2 }, { 8, 4, 0 }, { 7, 4, 1 },
	{ 6, 3, 0 }, { 6, 4, 2 }, { 5, 4, 3 }, { 2, 1, 0 },
};

static char display01[11] = "   |   |   ";
static char display02[11] = " 1 | 2 | 3 ";
static char display03[11] = "   |   |   ";
static char display04[11] = "   |   |   ";
static char display05[11] = " 4 | 5 | 6 ";
static char display06[11] = "   |   |   ";
static char display07[11] = "   |   |   ";
static char display08[11] = " 7 | 8 | 9 ";
static char display09[11] = "   |   |   ";
static char nought[9] = " O O O O ";
static char cross[9] = "X X X X X";

static char board[9] = "         ";
static char user;
static char cpu;

int main(int argc, char *argv[])
{
	user = yes_no("Do you want the first move?") == 'y' ? 'X' : 'O';
	int level = yes_no("Do you want a hard game?") == 'y' ? 1 : 0;
	cpu = user == 'X' ? 'O' : 'X';

	printf("\n");
	init(level);
	if (user == 'O') {
		make_move();
	}

	do {
		printf("\n");
		draw_board();
		get_user_input();
		draw_board();
		find_winner();
		make_move();
		draw_board();
		find_winner();
	} while (1);
}

static char yes_no(char *prompt)
{
	int show_prompt = 1;
	char response = ' ';

	for EVER {
		if (show_prompt) {
			printf("%s > ", prompt);
			show_prompt = 0;
		}

		int key = getchar();
		if (key == '\n') {
			if (response != ' ') {
				break;
			}

			show_prompt = 1;
		} else if (key == 'y' || key == 'Y') {
			response = 'y';
		} else if (key == 'n' || key == 'N') {
			response = 'n';
		}
	}

	return response;
}

static void init(int level)
{
	srand(time(NULL));

	if (level) {
		// Hard game,
		for (int i = 0; i < 100; i++) {
			int x, y, t;

			// Shuffle corners
			x = (rand() & 3) + 1;
			y = (rand() & 3) + 1;
			t = preferred[x];
			preferred[x] = preferred[y];
			preferred[y] = t;

			// Shuffle the rest
			x = (rand() & 3) + 5;
			y = (rand() & 3) + 5;
			t = preferred[x];
			preferred[x] = preferred[y];
			preferred[y] = t;
		}
	} else {
		int x;

		for (int i = 0; i < 100; i++) {
			int y, t;

			// Shuffle all
			x = (rand() & 7) + 1;
			y = (rand() & 7) + 1;
			t = preferred[x];
			preferred[x] = preferred[y];
			preferred[y] = t;
		}

		// Pick middle move
		x = (rand() & 7) + 1;
		preferred[0] = preferred[x];
		preferred[x] = 4;
	}
}

static void draw_board(void)
{
	set_cell(board[0], display01, display02, display03, 0);
	set_cell(board[1], display01, display02, display03, 1);
	set_cell(board[2], display01, display02, display03, 2);
	set_cell(board[3], display04, display05, display06, 0);
	set_cell(board[4], display04, display05, display06, 1);
	set_cell(board[5], display04, display05, display06, 2);
	set_cell(board[6], display07, display08, display09, 0);
	set_cell(board[7], display07, display08, display09, 1);
	set_cell(board[8], display07, display08, display09, 2);

	printf("%s\n", display01);
	printf("%s\n", display02);
	printf("%s\n", display03);
	printf("---+---+---\n");
	printf("%s\n", display04);
	printf("%s\n", display05);
	printf("%s\n", display06);
	printf("---+---+---\n");
	printf("%s\n", display07);
	printf("%s\n", display08);
	printf("%s\n", display09);
}

static void set_cell(char piece, char *l1, char *l2, char *l3, int index)
{
	static int offsets[3] = { 0, 4, 8 };
	char *p;

	if (piece == ' ') {
		return;
	}

	l1 += offsets[index];
	l2 += offsets[index];
	l3 += offsets[index];
	p = piece == 'O' ? nought : cross;
	*l1++ = *p++;
	*l1++ = *p++;
	*l1++ = *p++;
	*l2++ = *p++;
	*l2++ = *p++;
	*l2++ = *p++;
	*l3++ = *p++;
	*l3++ = *p++;
	*l3 = *p;
}

static void make_move(void)
{
	int move;

	/* can I win? */
	if ((move = find_hole(cpu)) != -1) {
		board[move] = cpu;
		return;
	}

	/* ok then ... can I block? */
	if ((move = find_hole(user)) != -1) {
		board[move] = cpu;
		return;
	}

	/* no win, no block - select next move from rnd list */
	for (int i = 0; i < 9; i++) {
		move = preferred[i];
		if (board[move] == ' ') {
			board[move] = cpu;
			return;
		}
	}

	/* huh?  No valid moves ... */
	printf("Out of moves - game is a draw ...\n");
	exit(0);
}

static int find_hole(char color)
{
	for (int i = 0; i < 8; i++) {
		int x, y, z;

		x = move_matrix[i][0];
		y = move_matrix[i][1];
		z = move_matrix[i][2];

		if (board[x] == color &&
		    board[y] == color &&
		    board[z] == ' ')
			return z;

		if (board[x] == color &&
		    board[y] == ' ' &&
		    board[z] == color)
			return y;

		if (board[x] == ' ' &&
		    board[y] == color &&
		    board[z] == color)
			return x;
	}

	return -1;
}

static void find_winner(void)
{
	for (int i = 0; i < 8; i++) {
		int x, y, z;

		x = move_matrix[i][0];
		y = move_matrix[i][1];
		z = move_matrix[i][2];

		/* human wins? */
		if (board[x] == user &&
		    board[y] == user &&
		    board[z] == user) {
			printf("\nCongratulations - you win (%d-%d-%d)!\n", x + 1, y + 1, z + 1);
			exit(0);
		}

		/* cpu wins? */
		if (board[x] == cpu &&
		    board[y] == cpu &&
		    board[z] == cpu) {
			printf("\nYay! I win (%d-%d-%d)!\n", x + 1, y + 1, z + 1);
			exit(0);
		}
	}

	// Is it a draw
	for (int i = 0; i < 9; i++) {
		if (board[i] == ' ') {
			return;
		}
	}

	printf("\nGame is a draw.\n");
	exit(0);
}

static void get_user_input(void)
{
	int move = ' ';
	do {
		int key = getchar();
		if (key != '\n') {
			move = key;
			continue;
		}

		if (move < '1' || move > '9') {
			printf("Illegal move - must be between 1 and 9\n");
			continue;
		}

		move -= '1';
		if (board[move] != ' ') {
			printf("You can't move to %c\n", move + 1);
			continue;
		}

		board[move] = user;
		return;
	} while (1);
}
