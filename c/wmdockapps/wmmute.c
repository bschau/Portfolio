#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include "wmdockapps.h"
#include "res/vol0.xpm"
#include "res/vol1.xpm"
#include "res/vol2.xpm"
#include "res/vol3.xpm"

/* macros */
#define INNERW 50

/* protos */
static void update_volume_level(void);
static void cycle_volume_level(int x, int y);
static void mute_sound(int x, int y);
static void run_volume_command(int volumne);

/* globals */
static char *usage = "Usage: wmMute [OPTIONS]\n\n" \
	"[OPTIONS]\n" \
	"  -c helper   Helper program\n" \
	"  -h          This page\n" \
	"  -l level    Start volume level\n";
static wm_cmds cmds = { update_volume_level, cycle_volume_level, mute_sound };
static char *cmd_helper = NULL;
static int vol_level = 0, cmd_token;
static Pixmap pixs[4];

int main(int argc, char *argv[])
{
	char *cmd = "wmmute-helper";
	int opt;

	while ((opt = getopt(argc, argv, "c:hl:")) != -1) {
		switch (opt) {
		case 'c':
			cmd_helper = optarg;
			break;

		case 'h':
			wm_usage(0, usage);

		case 'l':
			vol_level = atoi(optarg);
			break;

		default:
			wm_usage(1, usage);
		}
	}

	opt = strlen(cmd);
	if ((cmd_helper = malloc(opt + 3)) == NULL) {
		wm_error("Out of memory");
	}

	memmove(cmd_helper, cmd, opt);
	cmd_helper[opt++] = ' ';
	cmd_token = opt;
	cmd_helper[opt++] = ' ';
	cmd_helper[opt] = 0;

	wm_init_dock("wmmute", WM_STD_DOCK_WINDOW_SIZE, INNERW, &cmds);

	XpmCreatePixmapFromData(display, DefaultRootWindow(display), vol0_xpm, &pixs[0], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), vol1_xpm, &pixs[1], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), vol2_xpm, &pixs[2], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), vol3_xpm, &pixs[3], NULL, NULL);

	run_volume_command(vol_level);

	wm_event_loop();
	exit(0);
}

static void cycle_volume_level(int x, int y)
{
	run_volume_command(++vol_level);
}

static void mute_sound(int x, int y)
{
	run_volume_command(0);
}

static void update_volume_level(void)
{
	if (!display) {
		return;
	}

	XSetWindowBackgroundPixmap(display, dock_icon_window, pixs[vol_level & 3]);
	XClearWindow(display, dock_icon_window);
	XFlush(display);
}

static void run_volume_command(int volume)
{
	vol_level = volume & 3;

	cmd_helper[cmd_token] = vol_level + '0';
	int result = system(cmd_helper);
	if (!result) {
		fprintf(stderr, "Failed to run '%s': %i\n", cmd_helper, result);
		return;
	}

	update_volume_level();
}
