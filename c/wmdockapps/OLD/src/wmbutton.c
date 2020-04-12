/*
	wmbutton (c) 2010- Brian Schau <brian@schau.dk>

	wmbutton places a dock application which works on a button - actions
	are triggered depending on how the button is pressed.

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
#include <wmdockapps.h>
#include "res/empty.xpm"

/* macros */
#define MAXLINELEN 1024
#define INNERW 52

/* struct */
struct action {
	char *command;
	int blocking;
	char *gfx;
	Pixmap pix;
};

/* protos */
static void load_configuration(char *config);
static void update_status(void);
static void left_button_press(int x, int y);
static void right_button_press(int x, int y);
static void button_press(struct action *desc);

/* globals */
static char *usage = "Usage: wmbutton [OPTIONS] config-file\n\n" \
	"[OPTIONS]\n" \
	"  -h             This page\n";
static wm_cmds cmds = { update_status, left_button_press, right_button_press };
static struct action left, right;
static char *button_pixmap;
static Pixmap button_pix, empty_pix;
extern int errno;

/**
 * @param argc Number of arguments.
 * @param argv Argument list.
 */
int main(int argc, char *argv[]) {
	int opt;

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
	} else {
		fprintf(stderr, "No configuration file given\n");
		wm_usage(1, usage);
	}

	wm_init_dock("wmbutton", WM_STD_DOCK_WINDOW_SIZE, INNERW, &cmds);

	wm_install_sigchld_handler(NULL);

	if (button_pixmap) {
		XpmReadFileToPixmap(display, DefaultRootWindow(display), button_pixmap, &button_pix, NULL, NULL);
	} else {
		XpmCreatePixmapFromData(display, DefaultRootWindow(display), empty_xpm, &empty_pix, NULL, NULL);
		button_pix = empty_pix;
	}

	if (left.gfx) {
		XpmReadFileToPixmap(display, DefaultRootWindow(display), left.gfx, &left.pix, NULL, NULL);
	}

	if (right.gfx) {
		XpmReadFileToPixmap(display, DefaultRootWindow(display), right.gfx, &right.pix, NULL, NULL);
	}

	update_status();
	
	wm_event_loop();

	exit(0);
}

/**
 * @param config Configuration file.
 */
static void load_configuration(char *config) {
	FILE *f = fopen(config, "rb");
	int line = 0;
	char buffer[MAXLINELEN + 2];
	char *s, *token, *value;

	if (f == NULL) {
		wm_error("Cannot open configuration file '%s'\n", config);
		exit(1);
	}

	while ((s = fgets(buffer, sizeof(buffer), f)) != NULL) {
		line++;

		/* skip whites */
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

		if (strcasecmp(token, "left.command") == 0) {
			left.command = value;
		} else if (strcasecmp(token, "right.command") == 0) {
			right.command = value;
		} else if (strcasecmp(token, "left.pixmap") == 0) {
			left.gfx = value;
		} else if (strcasecmp(token, "right.pixmap") == 0) {
			right.gfx = value;
		} else if (strcasecmp(token, "left.blocking") == 0) {
			if ((strcasecmp(value, "true") == 0) ||
			    (strcasecmp(value, "yes") == 0) ||
			    (strcasecmp(value, "1") == 0)) {
				left.blocking = 1;
			} else {
				left.blocking = 0;
			}

			free(value);
		} else if (strcasecmp(token, "right.blocking") == 0) {
			if ((strcasecmp(value, "true") == 0) ||
			    (strcasecmp(value, "yes") == 0) ||
			    (strcasecmp(value, "1") == 0)) {
				right.blocking = 1;
			} else {
				right.blocking = 0;
			}

			free(value);
		} else if (strcasecmp(token, "pixmap") == 0) {
			button_pixmap = value;
		} else {
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
		XSetWindowBackgroundPixmap(display, dock_icon_window, button_pix);
		XClearWindow(display, dock_icon_window);
		XFlush(display);
	}
}

/**
 * Left mouse button handler.
 *
 * @param x X coordinate of mouse press.
 * @param y Y coordinate of mouse press.
 */
static void left_button_press(int x, int y) {
	button_press(&left);
}

/**
 * Right mouse button handler.
 *
 * @param x X coordinate of mouse press.
 * @param y Y coordinate of mouse press.
 */
static void right_button_press(int x, int y) {
	button_press(&right);
}

/**
 * @param desc Button description.
 */
static void button_press(struct action *desc) {
	if (desc->pix) {
		button_pix = desc->pix;
	}

	update_status();

	if (desc->blocking) {
		if (desc->command) {
			system(desc->command);
		}
	} else {
		if (desc->command) {
			wm_spawn(desc->command);
		}
	}
}
