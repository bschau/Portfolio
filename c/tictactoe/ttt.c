/*
 * Compile with: gcc -O2 -Wall -o ttt ttt.c
 *
 * Run:
 * 	ttt x e			user starts easy game
 * 	ttt x h			user starts hard game
 * 	ttt o e			cpu starts easy game
 * 	ttt o h			cpu starts hard game
 */
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void game_init(int level);
static void game_draw_board(void);
static void game_make_move(void);
static int game_find_hole(char color);
static void game_find_winner(void);
static void game_get_user_input(void);

static int preferred[9] = { 4, 8, 6, 2, 0, 7, 5, 3, 1 };
static unsigned short move_matrix[8][3] = {
	{ 8, 7, 6 }, { 8, 5, 2 }, { 8, 4, 0 }, { 7, 4, 1 },
	{ 6, 3, 0 }, { 6, 4, 2 }, { 5, 4, 3 }, { 2, 1, 0 },
};

static char board[9] = "         ";
static char user;
static char cpu;

int main(int argc, char *argv[])
{
	int level;

	if (argc != 3) {
		fprintf(stderr, "Usage: ttt x|o e|h\n");
		exit(1);
	}

	if (argv[1][0] == 'x') {
		user = 'X';
	} else if (argv[1][0] == 'o') {
		user = 'O';
	} else {
		fprintf(stderr, "Use x or o to specify user side\n");
		exit(1);
	}

	cpu = user == 'X' ? 'O' : 'X';

	if (argv[2][0] == 'e') {
		level = 0;
	} else if (argv[2][0] == 'h') {
		level = 1;
	} else {
		fprintf(stderr, "Use e or h to specify level\n");
		exit(1);
	}

	game_init(level);
	if (user == 'O') {
		game_make_move();
	}

	do {
		game_draw_board();
		game_get_user_input();
		game_find_winner();
		game_make_move();
		game_find_winner();
	} while (1);
}

void game_init(int level)
{
	for (int i = 0; i < 9; i++) {
		board[i] = ' ';
	}

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

static void game_draw_board(void)
{
	char cells[9] = "         ";
	int index, y;

	for (index = 0; index < 9; index++) {
		cells[index] = board[index] == ' ' ? '1' + index : board[index];
	}

	index = 0;
	for (y = 0; y < 3; y++) {
		printf("   |   |\n");
		printf(" %c | %c | %c\n", cells[index], cells[index + 1], cells[index + 2]);
		printf("   |   |\n");

		if (y != 2) {
			printf("---+---+---\n");
		}

		index += 3;
	}
}

static void game_make_move(void)
{
	int move;

	/* can I win? */
	if ((move = game_find_hole(cpu)) != -1) {
		board[move] = cpu;
		return;
	}

	/* ok then ... can I block? */
	if ((move = game_find_hole(user)) != -1) {
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

static int game_find_hole(char color)
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

static void game_find_winner(void)
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
			printf("Congratulations - you win!\n");
			exit(0);
		}

		/* cpu wins? */
		if (board[x] == cpu &&
		    board[y] == cpu &&
		    board[z] == cpu) {
			printf("Yay! I win!\n");
			exit(0);
		}
	}

	// Is it a draw
	for (int i = 0; i < 9; i++) {
		if (board[i] == ' ') {
			return;
		}
	}

	printf("Game is a draw.\n");
	exit(0);
}

static void game_get_user_input(void)
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
