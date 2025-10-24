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
#define HOME '@'
#define BOMBED '#'
#define BOMB_DELAY_RESET 4
#define BOMB_ANIMATING_RESET 80

static void handle_input(void);
static void tick(int signum);
static void start_bombing(void);
static void animate_bomb(void);
static void game_over(void);

static char *bomb_animation = "/-\\|/-\\|*****";
static char *playfield;
static int bomb_frame, bomb_x, bomb_y, bomb_animating, bomb_delay;
static int score, player_x, player_y, dying;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	player_x = 0;
	player_y = SCREENH / 2;
	bomb_x = -1;
	bomb_y = -1;
	bomb_animating = 100;
	bomb_frame = 0;
	bomb_delay = BOMB_DELAY_RESET;
	dying = 0;
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
	memset(p, ' ', SCREENW);
	p += SCREENW + 1;
	for (int i = 1; i < SCREENH; i++) {
		memset(p, '.', SCREENW);
		*p = ' ';
		p += SCREENW + 1;
	}

	playfield[((SCREENW + 1) * (SCREENH / 2)) + SCREENW - 1] = HOME;

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

	game_over();
}

static void handle_input(void)
{
	int c = getch();
	if (c == KEY_LEFT && player_x > 0) {
		player_x--;
	} else if (c == KEY_RIGHT && player_x < SCREENW - 1) {
		player_x++;
	} else if (c == KEY_UP && player_y > 1) {
		player_y--;
	} else if (c == KEY_DOWN && player_y < SCREENH - 1) {
		player_y++;
	}

	char *p = playfield + (player_y * (SCREENW + 1)) + player_x;
	switch (*p) {
		case '.':
			score += 2;
			*p = ' ';
			break;

		case HOME:
			game_over();
			break;

		case '*':
			dying = 1;
			break;

		case '#':
			score -= 3;
			break;

		case ' ':
			break;

		default:
			score--;
			break;
	}
}

static void tick(int signum)
{
	if (dying > 0) {
		if (dying > 200) {
			game_over();
			return;
		}

		dying++;
	} else {
		animate_bomb();
	}

	char scorestr[SCREENW + 1];
	snprintf(scorestr, sizeof(scorestr), "Score: %d", score);
	int len = strlen(scorestr);
	memmove(playfield + ((SCREENW - len) / 2), scorestr, len);

	attron(COLOR_PAIR(1));
	char *p = playfield;
	for (int y = 0; y < SCREENH; y++) {
		mvprintw(y, 0, "%s", p);
		p += SCREENW + 1;
	}

	if (dying > 0) {
		int d = dying / 20;
		char background = *(playfield + (player_y * (SCREENW + 1)) + player_x);
		mvaddch(player_y, player_x, d % 2 ? background : 'V');
	} else {
		mvaddch(player_y, player_x, 'V');
	}

	attroff(COLOR_PAIR(1));
	refresh();
}

static void start_bombing(void)
{
	bomb_frame = 0;
	bomb_delay = BOMB_DELAY_RESET;
	bomb_animating = BOMB_ANIMATING_RESET;

	do
	{
		bomb_x = player_x + (rand() % 7) - 3;
		if (bomb_x < 1) {
			bomb_x = 1;
		} else if (bomb_x > (SCREENW - 1)) {
			bomb_x = SCREENW - 1;
		}
		bomb_y = player_y + (rand() % 7) - 3;
		if (bomb_y < 1) {
			bomb_y = 1;
		} else if (bomb_y > (SCREENH - 1)) {
			bomb_y = SCREENH - 1;
		}
	} while (*(playfield + ((bomb_y * (SCREENW - 1)) + bomb_x)) == HOME);
}

static void animate_bomb(void)
{
	if (bomb_animating < 1) {
		start_bombing();
		return;
	}

	bomb_animating--;
	if (bomb_x == -1 || bomb_y == -1) {
		return;
	}

	if (bomb_delay > 0) {
		bomb_delay--;
		return;
	}
	bomb_delay = BOMB_DELAY_RESET;

	char c;
	if (bomb_frame < strlen(bomb_animation)) {
		c = bomb_animation[bomb_frame];
		bomb_frame++;
	} else {
		c = BOMBED;
	}

	*(playfield + (bomb_y * (SCREENW + 1)) + bomb_x) = c;
}

static void game_over(void)
{
	endwin();
	printf("You scored: %d\n\n", score);
	exit(0);
}
