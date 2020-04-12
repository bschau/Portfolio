/*
	wmmute (c) 2010- Brian Schau <brian@schau.dk>

	wmmute is a dock application to (somewhat) control sound settings.

	See the file LICENSE included in this package for license terms.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA 02111-1307
		USA
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <wmdockapps.h>
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
static char *cmd_helper=NULL;
static int vol_level=0, cmd_token;
static Pixmap pixs[4];

/**
 * @param argc Number of arguments.
 * @param argv Argument list.
 * Main.
 */
int main(int argc, char *argv[]) {
	char *cmd = "wmmute-helper";
	int opt;

	while ((opt = getopt(argc, argv, "c:hl:")) != -1) {
		switch (opt) {
		case 'c':
			cmd_helper=optarg;
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

/**
 * Cycle volume level 0 -> 3 (repeat).
 *
 * @param x X coordinate of button press.
 * @param y Y coordinate of button press.
 */
static void cycle_volume_level(int x, int y) {
	run_volume_command(++vol_level);
}

/**
 * Mute sound.
 *
 * @param x X coordinate of button press.
 * @param y Y coordinate of button press.
 */
static void mute_sound(int x, int y) {
	run_volume_command(0);
}

/**
 * Update the dock icon graphics based on the volume level.
 */
static void update_volume_level(void) {
	if (display) {
		XSetWindowBackgroundPixmap(display, dock_icon_window, pixs[vol_level & 3]);
		XClearWindow(display, dock_icon_window);
		XFlush(display);
	}
}

/**
 * Run the wmmute-helper program.
 *
 * @param volume Set this volume.
 */
static void run_volume_command(int volume) {
	vol_level = volume & 3;

	cmd_helper[cmd_token] = vol_level + '0';
	system(cmd_helper);
	update_volume_level();
}
