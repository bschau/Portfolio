/**
	wmbattery (c) 2010- Brian Schau <brian@schau.dk>

	wmbattery is a dock application shows the current battery level.

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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <wmdockapps.h>
#include "res/battery.xpm"

/* macros */
#define LINELEN 128

/* protos */
static void alarm_handler(int signal);
static void update_battery_status(void);
static int get_value(char *file, char *key);

/* globals */
static char *usage = "Usage: wmbattery [OPTIONS]\n\n" \
	"[OPTIONS]\n" \
	"  -b path      Path to battery\n" \
	"  -h           This page\n";
static wm_cmds cmds = { update_battery_status, NULL, NULL };
static char *path_info = NULL, *path_state = NULL;
static Pixmap battery_pix;
static GC dock_icon_gc;

/**
 * @param signal Signal received.
 */
static void alarm_handler(int signal) {
	update_battery_status();
	alarm(60);
}

/**
 * @param argc Number of arguments.
 * @param argv Argument list.
 */
int main(int argc, char *argv[]) {
	char *battery = "BAT0";
	int len = strlen("/proc/acpi/battery/") + 10, opt;

	while ((opt = getopt(argc, argv, "b:h")) != -1) {
		switch (opt) {
		case 'b':
			battery = optarg;
			break;

		case 'h':
			wm_usage(0, usage);

		default:
			wm_usage(1, usage);
		}
	}

	len += strlen(battery);
	if ((path_info = (char *) calloc(1, len)) == NULL) {
		wm_error("Out of memory");
	}

	if ((path_state = (char *) calloc(1, len)) == NULL) {
		wm_error("Out of memory");
	}

	snprintf(path_info, len, "/proc/acpi/battery/%s/info", battery);
	snprintf(path_state, len, "/proc/acpi/battery/%s/state", battery);

	wm_init_dock("wmbattery", WM_STD_DOCK_WINDOW_SIZE, WM_STD_ICON_WINDOW_SIZE, &cmds);
	dock_icon_gc = wm_get_gc();

	wm_install_alarm_handler(alarm_handler);

	XpmCreatePixmapFromData(display, DefaultRootWindow(display), battery_xpm, &battery_pix, NULL, NULL);

	XClearWindow(display, dock_icon_window);
	alarm(1);
	
	wm_event_loop();

	alarm(0);
	exit(0);
}

/**
 * Update battery status.
 */
static void update_battery_status(void) {
	float max = ((float) get_value(path_info, "last full capacity:")) + 0.01f;
	float cur = (float) get_value(path_state, "remaining capacity:");
	int level = (int) (((cur * 100.0f) / max) + 0.5f), h;
	Pixmap bat = XCreatePixmap(display, dock_icon_window, WM_STD_ICON_WINDOW_SIZE, WM_STD_ICON_WINDOW_SIZE, DefaultDepth(display, 0));

	XCopyArea(display, battery_pix, bat, dock_icon_gc, 0, 0, WM_STD_ICON_WINDOW_SIZE, WM_STD_ICON_WINDOW_SIZE, 0, 0);

	h = 30 - ((level * 30) / 100);
	if (h > 0) {
		XSetForeground(display, dock_icon_gc, wm_get_black_pixel_colour());
		XFillRectangle(display, bat, dock_icon_gc, 16, 11, 15, h);
	}

	XSetWindowBackgroundPixmap(display, dock_icon_window, bat);

	XClearWindow(display, dock_icon_window);
	XFreePixmap(display, bat);
	XFlush(display);
}

/**
 * Look for key in the given file and return value.
 *
 * @param file File to look in.
 * @param key Key to look for.
 * @return value.
 */
static int get_value(char *file, char *key) {
	int k_len = strlen(key), v = 0;
	FILE *f = fopen(file, "rb");
	char line[LINELEN + 2];
	char *s;

	if (f == NULL) {
		wm_error("Cannot open %s", file);
	}

	while ((s = fgets(line, LINELEN, f)) != NULL) {
		if (strncmp(s, key, k_len) == 0) {
			s += k_len;
			for (; *s && (isspace((int) *s)); s++);
			sscanf(s, "%i", &v);
			fclose(f);
			return v;
		}
	}

	wm_error("Key '%s' not found in %s", key, file);
	return 0; /* shut up gcc */
}
