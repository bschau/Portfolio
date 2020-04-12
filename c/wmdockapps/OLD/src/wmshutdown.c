/*
	wmshutdown (c) 2010- Brian Schau <brian@schau.dk>

	A shutdown/reboot button (with confirmation).

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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <WINGs/WINGs.h>
#include <wmdockapps.h>
#include "res/shutdown.xpm"

/* macros */
#define MAXLINELEN 1024
#define ALERTW 500
#define ALERTH 150

/* protos */
static void load_configuration(char *config);
static void update_status(void);
static void button_press(int x, int y);

/* globals */
static char *usage = "Usage: wmshutdown [OPTIONS] config-file\n\n" \
	"[OPTIONS]\n" \
	"  -h             This page\n";
static char *a_title = "Shutdown or Reboot?";
static char *a_message = "Do you want to shutdown or reboot?";
static char *a_bshutdown = "Shutdown";
static char *a_breboot = "Reboot";
static char *a_bcancel = "Cancel";
static char *a_cshutdown = "sudo /sbin/halt";
static char *a_creboot = "sudo /sbin/reboot";
static struct {
	char *token;
	char **dst;
} alert[] = {
	{ "title", &a_title },
	{ "message", &a_message },
	{ "shutdown.button", &a_bshutdown },
	{ "reboot.button", &a_breboot },
	{ "cancel.button", &a_bcancel },
	{ "shutdown.command", &a_cshutdown },
	{ "reboot.command", &a_creboot },
	{ NULL, NULL }
};
static wm_cmds cmds = { update_status, button_press, button_press };
static Pixmap shutdown_pix;
static WMScreen *screen;
extern int errno;

/**
 * @param argc Number of arguments.
 * @param argv Argument list.
 */
int main(int argc, char *argv[]) {
	int opt;

	WMInitializeApplication("wmshutdown", &argc, argv);

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
		case 'h':
			wm_usage(0, usage);

		default:
			wm_usage(1, usage);
		}
	}

	if ((argc - optind) == 1) {
		load_configuration(argv[optind]);
	}

	wm_init_dock("wmshutdown", WM_STD_DOCK_WINDOW_SIZE, WM_STD_ICON_WINDOW_SIZE, &cmds);

	XpmCreatePixmapFromData(display, DefaultRootWindow(display), shutdown_xpm, &shutdown_pix, NULL, NULL);

	update_status();
	
	wm_event_loop();

	exit(0);
}

/**
 * @param config Configuration file.
 */
static void load_configuration(char *config) {
	FILE *f = fopen(config, "rb");
	int line = 0, idx;
	char buffer[MAXLINELEN + 2];
	char *s, *token, *value;

	if (f == NULL) {
		wm_error("Cannot open configuration file '%s'\n", config);
		exit(1);
	}

	while ((s = fgets(buffer, sizeof(buffer), f)) != NULL) {
		line++;

		for (; (*s) && (isspace((int) *s)); s++);
		if (!*s) {
			continue;
		}

		if (*s == '#') {
			continue;
		}

		token = s;
		for (; (*s) && (isalnum((int) *s) || (*s == '.')); s++);
		if (!*s) {
			wm_error("Token with no value on line %i\n", line);
		}

		*s++ = 0;

		for (; (*s) && (isspace((int) *s)); s++);
		if (!*s) {
			wm_error("Token with no value on line %i\n", line);
		}

		value = s;
		for (; *s; s++);
		for (s--; isspace(*s); s--) {
			*s = 0;
		}

		if ((value = strdup(value)) == NULL) {
			wm_error("Out of memory");
		}

		for (idx = 0; alert[idx].token; idx++) {
			if (strcasecmp(alert[idx].token, token) == 0) {
				*alert[idx].dst = value;
				break;
			}
		}

		if (!alert[idx].token) {
			wm_error("Unknown token '%s' on line %i\n", token, line);
		}
	}

	fclose(f);
}

/**
 * Update the graphics in the dock_window with the current status.
 */
static void update_status(void) {
	if (display) {
		XSetWindowBackgroundPixmap(display, dock_icon_window, shutdown_pix);
		XClearWindow(display, dock_icon_window);
		XFlush(display);
	}
}

/**
 * Mouse button handler.
 *
 * @param x X coordinate of mouse press.
 * @param y Y coordinate of mouse press.
 */
static void button_press(int x, int y) {
	char *env_display = getenv("DISPLAY");
	WMAlertPanel *panel;
	Window parent;
	int result;

	if (env_display == NULL) {
		env_display = "";
	}

	if (screen == NULL) {
		screen = WMOpenScreen(env_display);
	}

	XSynchronize(WMScreenDisplay(screen), 1);

	panel = WMCreateAlertPanel(screen, NULL, a_title, a_message, a_bcancel, a_breboot, a_bshutdown);

	parent = XCreateSimpleWindow(display, DefaultRootWindow(display), (WMScreenWidth(screen) - ALERTW) / 2, (WMScreenHeight(screen) - ALERTH) / 2, ALERTW, ALERTH, 0, 0, 0);

	WMMapWidget(panel->win);

	WMRunModalLoop(WMWidgetScreen(panel->win), WMWidgetView(panel->win));

	result = panel->result;

	WMUnmapWidget(panel->win);

	XDestroyWindow(display, parent);
	WMDestroyAlertPanel(panel);

	if (result > 0) {
		system(a_creboot);
	} else if (result < 0) {
		system(a_cshutdown);
	}
}
