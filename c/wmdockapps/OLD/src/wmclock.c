/**
	wmclock (c) 2010- Brian Schau <brian@schau.dk>

	wmclock is a dock application which provides a nice watch.

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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <wmdockapps.h>
#include "res/clock.xpm"
#include "bell.h"

/* protos */
static void alarm_handler(int signal);
static void *load_bell_file(char *name, int *size);
static void expose_event(void);
static void update_clock_arms(int h, int m);
static void set_arm(Pixmap pm, int x, int y);
static void bell(int cycle);

/* globals */
static char *usage = "Usage: wmclock [OPTIONS]\n\n" \
	"[OPTIONS]\n" \
	"  -H           Sound bell every half hour\n" \
	"  -S           Silent - do not sound bell\n" \
	"  -b bellfile  Sound file containing bell\n" \
        "  -h           This page\n" \
	"  -s seconds   Seconds to sleep between bells\n" \
	"  -u useconds  Microseconds to sleep between bells\n";
static wm_cmds cmds = { expose_event, NULL, NULL };
static const float PI=3.141592654F;
static void *bell_file = NULL;
static int bell_file_size = 0, silent = 0, secs = 0, u_secs = 0, half = 0;
static Pixmap clock_pix;
static GC dock_icon_gc;
extern int errno;

/**
 * @param signal Signal received.
 */
static void alarm_handler(int signal) {
	time_t now = time(NULL);
	struct tm *bt = localtime(&now);

	update_clock_arms(bt->tm_hour, bt->tm_min);

	if (!silent) {
		if (bt->tm_min == 30) {
			bell(1);
		} else if (bt->tm_min == 0) {
			bell(bt->tm_hour);
		}
	}

	now = time(NULL);
	bt = localtime(&now);
	alarm(((61 - bt->tm_sec) % 60) + 1);
}

/**
 * @param name Ding file to load.
 * @param size Where to store size of bell file
 * @return bell file or NULL (error).
 */
static void *load_bell_file(char *name, int *size) {
	void *dst;
	FILE *fh;
	long s;
	size_t items_read;

	*size = 0;
	if (name == NULL) {
		return NULL;
	}

	if ((fh = fopen(name, "r")) == NULL) {
		perror(name);
		return NULL;
	}

	fseek(fh, 0, SEEK_END);
	if ((s = ftell(fh)) < 1) {
		fprintf(stderr, "Cannot seek on %s\n", name);
		fclose(fh);
		return NULL;
	}

	if ((dst = (char *) calloc(s + 1, 1)) == NULL) {
		fprintf(stderr, "Out of memory\n");
		fclose(fh);
		return NULL;
	}

	rewind(fh);
	items_read = fread(dst, 1, s, fh);
	fclose(fh);
	
	if (items_read == s) {
		*size = s;
		return dst;
	}

	free(dst);
	return NULL;
}

/**
 * @param argc Number of arguments.
 * @param argv Argument list.
 */
int main(int argc, char *argv[]) {
	char *bell_file_name = NULL;
	int opt, x1, x2, y1, y2;

	while ((opt = getopt(argc, argv, "HSb:hs:u:")) != -1) {
		switch (opt) {
		case 'H':
			half = 1;
			break;

		case 'S':
			silent = 1;
			break;

		case 'b':
			bell_file_name = optarg;
			break;

		case 'h':
			wm_usage(0, usage);

		case 's':
			secs = atoi(optarg);
			break;

		case 'u':
			u_secs = atoi(optarg);
			break;

		default:
			wm_usage(1, usage);
		}
	}

	if (!silent) {
		if ((bell_file = load_bell_file(bell_file_name, &bell_file_size)) == NULL) {
			bell_file = bell_data;
			bell_file_size = bell_size;
		}
	}

	wm_init_dock("wmclock", WM_STD_DOCK_WINDOW_SIZE, WM_STD_ICON_WINDOW_SIZE, &cmds);

	wm_install_alarm_handler(alarm_handler);

	XpmCreatePixmapFromData(display, DefaultRootWindow(display), clock_xpm, &clock_pix, NULL, NULL);

	dock_icon_gc = wm_get_gc();

	/* create hour marks */
	for (opt = 0; opt < 12; opt++) {
		x1 = (int) (16.0 * sin((2 * PI * (11 - opt)) / 12) + (WM_STD_ICON_WINDOW_SIZE / 2));
		y1 = (int) (16.0 * cos((2 * PI * (11 - opt)) / 12) + (WM_STD_ICON_WINDOW_SIZE / 2));
		x2 = (int) (14.0 * sin((2 * PI * (11 - opt)) / 12) + (WM_STD_ICON_WINDOW_SIZE / 2));
		y2 = (int) (14.0 * cos((2 * PI * (11 - opt)) / 12) + (WM_STD_ICON_WINDOW_SIZE / 2));

		XDrawLine(display, clock_pix, dock_icon_gc, x1, y1, x2, y2);
	}

	XClearWindow(display, dock_icon_window);
	alarm(1);
	
	wm_event_loop();

	alarm(0);
	exit(0);
}

/**
 * Expose event handler (called from X-server).
 */
static void expose_event(void) {
	time_t now = time(NULL);
	struct tm *bt = localtime(&now);

	update_clock_arms(bt->tm_hour, bt->tm_min);
}

/**
 * Update the clock with the current arms values.
 *
 * @param h Hour arm.
 * @param m Minutes arm.
 */
static void update_clock_arms(int h, int m) {
	int m1 = m % 60;
	float c, x, y;
	Pixmap clock;

	h %= 12;
	m %= 60;

	clock = XCreatePixmap(display, dock_icon_window, WM_STD_DOCK_WINDOW_SIZE, WM_STD_DOCK_WINDOW_SIZE, DefaultDepth(display, 0));
	XCopyArea(display, clock_pix, clock, dock_icon_gc, 0, 0, WM_STD_DOCK_WINDOW_SIZE, WM_STD_DOCK_WINDOW_SIZE, 0, 0);

	c = ((((float) (h)) * 60.0F) + ((float) m1)) * 2.0 * PI;
	x = 8.0 * sin(c / (60.0 * 12.0));
	y = 8.0 * cos(c / (60.0 * 12.0));
	set_arm(clock, (int) x, (int) -y);

	c = ((float) m) * 2.0 * PI;
	x = 12.0 * sin(c / 60.0);
	y = 12.0 * cos(c / 60.0);
	set_arm(clock, (int)x, (int) -y);

	XSetWindowBackgroundPixmap(display, dock_icon_window, clock);
	XClearWindow(display, dock_icon_window);
	XFreePixmap(display, clock);
	XFlush(display);
}

/**
 * Draw a clock arm from center to x, y.
 *
 * @param pm Pixmap to draw in.
 * @param x Destination x.
 * @param y Destination y.
 */
static void set_arm(Pixmap pm, int x, int y) {
	x += (WM_STD_ICON_WINDOW_SIZE / 2);
	y += (WM_STD_ICON_WINDOW_SIZE / 2);
	XDrawLine(display, pm, dock_icon_gc, 24, 24, x, y);
	XDrawLine(display, pm, dock_icon_gc, 23, 23, x, y);
	XDrawLine(display, pm, dock_icon_gc, 25, 23, x, y);
	XDrawLine(display, pm, dock_icon_gc, 23, 25, x, y);
	XDrawLine(display, pm, dock_icon_gc, 25, 25, x, y);
}

/**
 * @param cycle How many time to ring the bell.
 */
static void bell(int cycle) {
	FILE *fh;
	int sel_stat;
	struct timeval time_out;

	if (!cycle) {
		cycle = 24;
	}

	if (half) {
		if (cycle > 12) {
			cycle -= 12;
		}
	}

	for (; cycle > 0; cycle--) {
		if ((fh = fopen("/dev/dsp", "w")) == NULL) {
			return;
		}

		fwrite(bell_file, 1, bell_file_size, fh);
		fclose(fh);

		if ((secs) || (u_secs)) {
			time_out.tv_sec = secs;
			time_out.tv_usec = u_secs;
			if ((sel_stat = select(0, NULL, NULL, NULL, &time_out)) != 0) {
				fprintf(stderr, "select error\n");
				break;
			}
		}
	}
}
