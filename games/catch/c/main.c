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

static void new_level(void);
static void handle_input(void);
static void tick(int signum);
static void handle_collision(void);

static char *playfield;
static int score, player_x, player_y, delay, delay_reset;
static volatile int flashing;
static char background;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	delay_reset = 20;
	score = -1;
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

	new_level();
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
		if (flashing) {
			continue;
		}
		handle_input();
	}

	endwin();
}

static void new_level(void)
{
	score++;
	flashing = 0;
	player_y = 1;
	player_x = rand() % SCREENW;
	delay = delay_reset;
	background = ' ';

	char *p = playfield;
	for (int i = 0; i < SCREENH - 1; i++) {
		memset(p, ' ', SCREENW);
		p += SCREENW + 1;
	}
	memset(p, '=', SCREENW);

	int bucket_x = rand() % (SCREENW -3);
	p = playfield + (19 * (SCREENW + 1)) + bucket_x;
	*p = ']';
	*(p + 1) = '_';
	*(p + 2) = '[';

	int shield_x = (rand() % 4) - 3 + bucket_x - 1;
	if (shield_x < 3) {
		shield_x = 3;
	} else if (shield_x > SCREENW - 3) {
		shield_x = SCREENW -3;
	}

	int shield_y = (rand() % 8) + 10;
	p = playfield + (shield_y * (SCREENW + 1)) + shield_x;
	*p = '=';
	*(p + 1) = '=';
	*(p + 2) = '=';

	if (delay_reset > 0) {
		delay_reset--;
	}
}

static void handle_input(void)
{
	int c = getch();
	if (flashing) {
		return;
	}

	if (c == KEY_LEFT && player_x > 0) {
		player_x--;
	} else if (c == KEY_RIGHT && player_x < SCREENW - 1) {
		player_x++;
	}
}

static void tick(int signum)
{
	if (flashing > 0) {
		if (flashing > 200) {
			if (background == '_') {
				new_level();
				return;
			}

			endwin();
			printf("You scored: %d\n\n", score);
			exit(0);
		}

		flashing++;
	} else {
		delay--;
		if (delay < 1) {
			delay = delay_reset;
			player_y++;
		}
	}

	char scorestr[SCREENW / 2];
	snprintf(scorestr, SCREENW / 2, "Score: %d", score);
	memmove(playfield, scorestr, strlen(scorestr));

	attron(COLOR_PAIR(1));
	char *p = playfield;
	for (int y = 0; y < SCREENH; y++) {
		mvprintw(y, 0, "%s", p);
		p += SCREENW + 1;
	}

	if (flashing > 0) {
		int d = flashing / 20;
		mvaddch(player_y, player_x, d % 2 ? background : 'o');
	} else {
		mvaddch(player_y, player_x, 'o');
	}

	attroff(COLOR_PAIR(1));
	refresh();

	if (!flashing) {
		handle_collision();
	}
}

static void handle_collision(void)
{
	char *p = playfield + (player_y * (SCREENW + 1)) + player_x;

	background = *p;
	if (background != ' ') {
		flashing = 1;
	}
}
