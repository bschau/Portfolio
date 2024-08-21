#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define EVER (;;)
#define FPS 60
#define CANVAS_PEN 1
#define BORDER_PEN (CANVAS_PEN + 1)
#define GRAVE_PEN (BORDER_PEN + 1)
#define ZOMBIE_PEN (GRAVE_PEN + 1)
#define PLAYER_PEN (ZOMBIE_PEN + 1)
#define HOLE_PEN (PLAYER_PEN + 1)
#define STATUS_PEN (HOLE_PEN + 1)
#define P_WIDTH (20)
#define P_HEIGHT (10)
#define MAX_GRAVES (20)
#define ZOMBIES (3)

struct zombie {
	int x;
	int y;
};

static int range_random(int limit);
static void handle_input(void);
static char P(int x, int y);
static char Z(int z, int adj_x, int adj_y);
static int is_game_over(void);
static void move_zombies(int move);
static void tick(int signum);
static void redraw(void);
static void pout(int x, int y, int color_pair, char c);

static char playfield[P_HEIGHT][P_WIDTH] = {
	{ "::::::::::::::::::::" },
	{ ":                  :" },
	{ ":                  :" },
	{ ":                  :" },
	{ ":                  :" },
	{ ":                  :" },
	{ ":                  :" },
	{ ":                  :" },
	{ ":                   " },
	{ "::::::::::::::::::::" }
};

static struct zombie zombies[ZOMBIES] = {
	{ 18, 2 },
	{ 18, 3 },
	{ 18, 4 }
};

int player_x = 1, player_y = 1, holes_left = 5, time_left = 60;
int fell_in_hole = 0, caught_by_zombie = 0, exited = 0;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	initscr();
	if (has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "Your terminal does not support color\n");
		exit(1);
	}
	start_color();
	nonl();
	cbreak();
	noecho();
	keypad(stdscr, true);
	init_pair(CANVAS_PEN, COLOR_BLACK, COLOR_BLACK);
	init_pair(BORDER_PEN, COLOR_BLACK, COLOR_YELLOW);
	init_pair(GRAVE_PEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(ZOMBIE_PEN, COLOR_WHITE, COLOR_BLACK);
	init_pair(PLAYER_PEN, COLOR_BLUE, COLOR_WHITE);
	init_pair(HOLE_PEN, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(STATUS_PEN, COLOR_RED, COLOR_BLACK);

	clear();
	curs_set(0);

	int grave_no = 0;
	while (grave_no < MAX_GRAVES) {
		int x = range_random(P_WIDTH - 2) + 1;
		int y = range_random(P_HEIGHT - 2) + 1;
		if (playfield[y][x] == '+') {
			continue;
		}

		playfield[y][x] = '+';
		grave_no++;
	}

	playfield[8][18] = ' ';
	refresh();

	struct sigaction action;
	action.sa_handler = &tick;
	action.sa_flags = SA_RESTART;
	sigaction(SIGALRM, &action, NULL);

	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 1000000 / FPS;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 1000000 / FPS;
	setitimer(ITIMER_REAL, &timer, NULL);

	while (!is_game_over()) {
		handle_input();
	}

	endwin();

	if (fell_in_hole) {
		printf("Yuck! You fell into one of your own holes.\n");
	} else if (caught_by_zombie) {
		printf("You were caught and eaten by a zombie!\n");
	} else if (exited) {
		printf("Yay! You escaped the graveyard!\n");
	} else {
		printf("Ding - ding - time ran out. You are doomed to stay here forever ...\n");
	}

	exit(0);
}

static int range_random(int limit)
{
	return rand() % limit;
}

static void handle_input(void)
{
	int move = getch();
	int x = player_x;
	int y = player_y;

	if (move == ' ' && holes_left > 0 && P(x, y) == ' ') {
		playfield[y][x] = 'O';
		holes_left--;
		return;
	} else if (move == KEY_LEFT) {
		x--;
	} else if (move == KEY_RIGHT) {
		x++;
	} else if (move == KEY_UP) {
		y--;
	} else if (move == KEY_DOWN) {
		y++;
	}

	if (x == player_x && y == player_y) {
		return;
	}

	char c = P(x, y);
	if (c == '+' || c == ':') {
		return;
	}

	if (c == 'O') {
		fell_in_hole = 1;
		return;
	}

	player_x = x;
	player_y = y;
	if (player_x == 19 && player_y == 8) {
		exited = 1;
		return;
	}

	move_zombies(move);
	for (int z = 0; z < ZOMBIES; z++) {
		if (zombies[z].x == player_x && zombies[z].y == player_y) {
			caught_by_zombie = 1;
			return;
		}
	}
	time_left--;
}

static char P(int x, int y)
{
	return playfield[y][x];
}

static char Z(int zombie, int adj_x, int adj_y)
{
	int x = zombies[zombie].x + adj_x;
	int y = zombies[zombie].y + adj_y;
	return playfield[y][x];
}

static int is_game_over(void)
{
	return fell_in_hole || caught_by_zombie || time_left == 0|| exited;
}

static void move_zombies(int move)
{
	for (int z = 0; z < ZOMBIES; z++) {
		if (move == KEY_DOWN && Z(z, 0, 1) == ' ') {
			zombies[z].y++;
		} else if (move == KEY_UP && Z(z, 0, -1) == ' ') {
			zombies[z].y--;
		} else if (move == KEY_LEFT) {
			if (player_x < zombies[z].x && Z(z, -1, 0) == ' ') {
				zombies[z].x--;
			} else if (player_x > zombies[z].x && Z(z, 1, 0) == ' ') {
				zombies[z].x++;
			}
		}
	}
}

static void tick(int signum)
{
	redraw();
}

static void redraw(void)
{
	for (int y = 0; y < P_HEIGHT; y++) {
		for (int x = 0; x < P_WIDTH; x++) {
			char c = playfield[y][x];
			switch (c) {
				case ':':
					pout(x, y, BORDER_PEN, ' ');
					break;
				case '+':
					pout(x, y, GRAVE_PEN, '+');
					break;
				case 'O':
					pout(x, y, HOLE_PEN, 'O');
					break;
				default:
					pout(x, y, CANVAS_PEN, ' ');
					break;
			}
		}
	}

	for (int z = 0; z < ZOMBIES ; z++) {
		pout(zombies[z].x, zombies[z].y, ZOMBIE_PEN, '#');
	}

	pout(player_x, player_y, PLAYER_PEN, '@');

	attron(COLOR_PAIR(STATUS_PEN));
	mvprintw(11, 0, "Holes left: %d  ", holes_left);
	mvprintw(12, 0, "Time left: %d  ", time_left);
	attroff(COLOR_PAIR(STATUS_PEN));

	refresh();
}

static void pout(int x, int y, int color_pair, char c)
{
	attron(COLOR_PAIR(color_pair));
	mvaddch(y, x, c);
	attroff(COLOR_PAIR(color_pair));
}
