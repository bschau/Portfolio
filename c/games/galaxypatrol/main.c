#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define EVER (;;)
#define FPS 60
#define SCREENW 32
#define SCREENH 22
#define PLAYER_Y 3

static void add_stars(char *p);
static void add_fuel(char *p);
static void handle_input(void);
static void tick(int signum);
static void scroll_stars(void);
static void handle_collision(void);

static char *playfield;
static int score, fuel, scroll_delay, scroll_reset, player_x, dying;
static int number_of_stars;
static char background;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	scroll_reset = 30;
	fuel = 150;
	player_x = SCREENW / 2;
	number_of_stars = 1;
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
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	clear();
	curs_set(0);

	playfield = calloc((SCREENW + 1) * SCREENH, sizeof(char));
	if (playfield == NULL) {
		perror("Failed to allocate playfield");
		exit(1);
	}

	char *p = playfield;
	for (int i = 0; i < SCREENH; i++) {
		memset(p, ' ', SCREENW);
		p += SCREENW + 1;
	}

	p = playfield + (8 * (SCREENW + 1));
	for (int i = 8; i < SCREENH; i++) {
		add_stars(p);
		p += SCREENW + 1;
	}

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

	for EVER {
		if (dying) {
			continue;
		}
		handle_input();
	}

	endwin();
}

static void add_stars(char *p)
{
	memset(p, ' ', SCREENW);
	for (int i = 0; i < number_of_stars; i++) {
		int x = rand() % SCREENW;
		*(p + x) = '*';
	}
}

static void add_fuel(char *p)
{
	int x = rand() % 100;
	if (x > 25) {
		return;
	}

	x = rand() % SCREENW;
	*(p + x) = '#';
}

static void handle_input(void)
{
	int c = getch();
	if (c == KEY_LEFT && player_x > 0) {
		player_x--;
	} else if (c == KEY_RIGHT && player_x < SCREENW - 1) {
		player_x++;
	}
}

static void tick(int signum)
{
	if (dying > 0) {
		if (dying > 200) {
			endwin();
			printf("You scored: %d\n\n", score);
			exit(0);
		}

		dying++;
	} else {
		scroll_stars();
	}

	char scorestr[SCREENW / 2], fuelstr[SCREENW / 2];
	memset(playfield, ' ', SCREENW);

	snprintf(scorestr, SCREENW / 2, "Score: %d", score);
	memmove(playfield, scorestr, strlen(scorestr));

	int size = snprintf(fuelstr, SCREENW / 2, "Fuel: %d", fuel);
	memmove(playfield + SCREENW - size, fuelstr, size);

	attron(COLOR_PAIR(1));

	char *p = playfield;
	for (int y = 0; y < SCREENH; y++) {
		mvprintw(y, 0, "%s", p);
		p += SCREENW + 1;
	}

	if (dying > 0) {
		int d = dying / 20;
		mvaddch(PLAYER_Y, player_x, d % 2 ? background : 'V');
	} else {
		mvaddch(PLAYER_Y, player_x, 'V');
	}

	attroff(COLOR_PAIR(1));
	refresh();

	if (!dying) {
		handle_collision();
	}
}

static void scroll_stars(void)
{
	if (scroll_delay > 0) {
		scroll_delay--;
		return;
	}

	scroll_delay = scroll_reset;
	int size = (SCREENW + 1) * (SCREENH - 2);
	memmove(playfield + SCREENW + 1, playfield + (2 * (SCREENW + 1)), size);
	char *p = playfield + size + SCREENW + 1;
	add_stars(p);
	add_fuel(p);
	score++;
	fuel--;
	if (fuel < 1) {
		background = ' ';
		dying = 1;
	}

	if (score % 25 == 0) {
		if (scroll_reset > 10) {
			scroll_reset--;
		}
	}

	int step = (score / 200) + 1;
	number_of_stars = step > 7 ? 7 : step;
}

static void handle_collision(void)
{
	char *p = playfield + (PLAYER_Y * (SCREENW + 1)) + player_x;
	char c = *p;

	if (c == '*') {
		background = '*';
		dying = 1;
	} else if (c == '#') {
		*p = ' ';
		fuel += 25;
	}
}
