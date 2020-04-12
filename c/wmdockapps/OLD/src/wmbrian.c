/**
	wmbrian (c) 2010- Brian Schau <brian@schau.dk>

	wmbrian is Brians special dock application

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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <wmdockapps.h>
#include "res/bat0.xpm"
#include "res/bat1.xpm"
#include "res/bat2.xpm"
#include "res/bat3.xpm"
#include "res/bat4.xpm"
#include "res/bat5.xpm"
#include "res/colon.xpm"
#include "res/empty.xpm"
#include "res/numbers.xpm"
#include "res/warning.xpm"
#include "res/wlan0.xpm"
#include "res/wlan1.xpm"
#include "res/wlan2.xpm"
#include "res/wlan3.xpm"
#include "res/wlan4.xpm"
#include "res/wlan5.xpm"

/* macros */
#define LINELEN 128
#define INNERW 52

/* protos */
static void alarm_handler(int signal);
static void update_status(void);
static int get_wireless_status(void);
static int get_battery_status(void);
static int get_value(char *file, char *key);
static void button_press(int x, int y);

/* globals */
static char *usage = "Usage: wmbrian [OPTIONS]\n\n" \
	"[OPTIONS]\n" \
	"  -B cmd       Command to run when clicking the battery\n" \
	"  -T cmd       Command to run when clicking the clock\n" \
	"  -W cmd       Command to run when clicking the signal strength\n" \
	"               (Default is /usr/bin/wicd-client -n)\n" \
	"  -b path      Path to battery\n" \
	"  -h           This page\n" \
	"  -w interface Wlan interface to watch (default: wlan0)\n";
static wm_cmds cmds = { update_status, button_press, NULL };
static char *path_info = NULL, *path_state = NULL;
static char *wlan = "wlan0", *wireless_cmd = "/usr/bin/wicd-client -n";
static char *battery_cmd = NULL, *time_cmd = NULL;
static Pixmap empty_pix, colon_pix, numbers_pix;
static Pixmap wlan_pix[6], bat_pix[7];
static GC dock_icon_gc;
static int wlan_error_seen;

/**
 * @param signal Signal received.
 */
static void alarm_handler(int signal) {
	update_status();
	alarm(60);
}

/**
 * @param argc Number of arguments.
 * @param argv Argument list.
 */
int main(int argc, char *argv[]) {
	char *battery = "BAT0";
	int len = strlen("/proc/acpi/battery/") + 10, opt;

	while ((opt = getopt(argc, argv, "B:T:W:b:hw:")) != -1) {
		switch (opt) {
		case 'B':
			battery_cmd = optarg;
			break;

		case 'T':
			time_cmd = optarg;
			break;

		case 'W':
			wireless_cmd = optarg;
			break;

		case 'b':
			battery = optarg;
			break;

		case 'h':
			wm_usage(0, usage);

		case 'w':
			wlan = optarg;
			break;

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

	wm_init_dock("wmbattery", WM_STD_DOCK_WINDOW_SIZE, INNERW, &cmds);

	wm_install_alarm_handler(alarm_handler);

	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bat0_xpm, &bat_pix[0], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bat1_xpm, &bat_pix[1], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bat2_xpm, &bat_pix[2], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bat3_xpm, &bat_pix[3], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bat4_xpm, &bat_pix[4], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bat5_xpm, &bat_pix[5], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), colon_xpm, &colon_pix, NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), empty_xpm, &empty_pix, NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), numbers_xpm, &numbers_pix, NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), warning_xpm, &bat_pix[6], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), wlan0_xpm, &wlan_pix[0], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), wlan1_xpm, &wlan_pix[1], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), wlan2_xpm, &wlan_pix[2], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), wlan3_xpm, &wlan_pix[3], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), wlan4_xpm, &wlan_pix[4], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), wlan5_xpm, &wlan_pix[5], NULL, NULL);

	dock_icon_gc = wm_get_gc();

	XClearWindow(display, dock_icon_window);
	alarm(1);
	
	wm_event_loop();

	alarm(0);
	exit(0);
}

/**
 * Update all status.
 */
static void update_status(void) {
	time_t now = time(NULL);
	struct tm *bt = localtime(&now);
	Pixmap pix;
	int x = 2, lvl;

	pix = XCreatePixmap(display, dock_icon_window, INNERW, INNERW, DefaultDepth(display, 0));

	XCopyArea(display, empty_pix, pix, dock_icon_gc, 0, 0, INNERW, INNERW, 0, 0);

	if (bt->tm_hour > 9) {
		XCopyArea(display, numbers_pix, pix, dock_icon_gc, ((bt->tm_hour / 10) * 9), 0, 9, 20, x, 29);
	}

	x += 11;
	XCopyArea(display, numbers_pix, pix, dock_icon_gc, ((bt->tm_hour % 10) * 9), 0, 9, 20, x, 29);
	
	x += 11;
	XCopyArea(display, colon_pix, pix, dock_icon_gc, 0, 0, 2, 20, x, 30);
	x += 4;

	XCopyArea(display, numbers_pix, pix, dock_icon_gc, ((bt->tm_min / 10) * 9), 0, 9, 20, x, 29);
	x += 11;
	XCopyArea(display, numbers_pix, pix, dock_icon_gc, ((bt->tm_min % 10) * 9), 0, 9, 20, x, 29);

	lvl = get_wireless_status();
	XCopyArea(display, wlan_pix[lvl], pix, dock_icon_gc, 0, 0, 24, 24, 2, 2);

	lvl = get_battery_status();
	XCopyArea(display, bat_pix[lvl], pix, dock_icon_gc, 0, 0, 24, 24, 27, 2);

	XSetWindowBackgroundPixmap(display, dock_icon_window, pix);

	XClearWindow(display, dock_icon_window);
	XFreePixmap(display, pix);
	XFlush(display);
}

/**
 * @return wireless status.
 */
static int get_wireless_status() {
	struct iw_statistics stats;
	struct iw_range range;
	struct iwreq req;
	int sfd, lvl;

	strncpy(req.ifr_name, wlan, sizeof(req.ifr_name));

	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		return 0;
	}

	req.u.data.pointer = &range;
	req.u.data.length = sizeof(struct iw_range);
	req.u.data.flags = 1;

	if (ioctl(sfd, SIOCGIWRANGE, &req) == -1) {
		close(sfd);
		return 0;
	}

	req.u.data.pointer = &stats;
	req.u.data.length = sizeof(struct iw_statistics);
	req.u.data.flags = 1;

	if (ioctl(sfd, SIOCGIWSTATS, &req) == -1) {
		close(sfd);
		return 0;
	}

	close(sfd);

	if (range.max_qual.qual) {
		lvl = (stats.qual.qual * 100) / range.max_qual.qual; 
		if (lvl > 90) {
			return 5;
		} else if (lvl > 75) {
			return 4;
		} else if (lvl > 60) {
			return 3;
		} else if (lvl > 45) {
			return 2;
		} else if (lvl > 25) {
			return 1;
		}
	} else {
		if (!wlan_error_seen) {
			wlan_error_seen = 1;
			fprintf(stderr, "No maximum signal strength available\n");
		}
	}

	return 0;

}

/**
 * @return battery status.
 */
static int get_battery_status(void) {
	float max = ((float) get_value(path_info, "last full capacity:")) + 0.01f;
	float cur = (float) get_value(path_state, "remaining capacity:");
	int lvl = (int) ((cur * 100.0) / max) ;

	if (lvl > 95) {
		return 5;
	}

	if (lvl > 75) {
		return 4;
	}

	if (lvl > 55) {
		return 3;
	}

	if (lvl > 35) {
		return 2;
	}

	if (lvl > 15) {
		return 1;
	}

	if (lvl > 5) {
		return 0;
	}

	return 6;
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

/**
 * @param x X coordinate of mouse press.
 * @param y Y coordinate of mouse press.
 */
static void button_press(int x, int y) {
	if (y >= 29) {
		if (time_cmd) {
			wm_spawn(time_cmd);
		}
	} else {
		if (x < 27) {
			if (wireless_cmd) {
				wm_spawn(wireless_cmd);
			}
		} else {
			if (battery_cmd) {
				wm_spawn(battery_cmd);
			}
		}
	}
}
