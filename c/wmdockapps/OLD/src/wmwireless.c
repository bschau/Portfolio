/**
	wmwireless (c) 2010- Brian Schau <brian@schau.dk>

	wmwireless is a dock application which shows the signal strength
	of the specified interface.

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
#include <sys/wait.h>
#include <linux/wireless.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <wmdockapps.h>
#include "res/bwlan0.xpm"
#include "res/bwlan1.xpm"
#include "res/bwlan2.xpm"
#include "res/bwlan3.xpm"
#include "res/bwlan4.xpm"
#include "res/bwlan5.xpm"

/* macros */
#define LINELEN 128
#define INNERW 52

/* protos */
static void sigchld_handler(int signal);
static void alarm_handler(int signal);
static void update_status(void);
static int get_wireless_status(void);
static void left_button_action(int x, int y);
static void right_button_action(int x, int y);

/* globals */
static char *usage = "Usage: wmwireless [OPTIONS]\n\n" \
	"[OPTIONS]\n" \
	"  -l cmd       Left mouse button command (default: /usr/bin/wicd-client -n)\n" \
	"  -h           This page\n" \
	"  -r cmd       Right mouse button command\n" \
	"  -w interface Wlan interface to watch (default: wlan0)\n";
static wm_cmds cmds = { update_status, left_button_action, right_button_action };
static char *lba_cmd = "/usr/bin/wicd-client -n", *rba_cmd = NULL;
static char *wlan = "wlan0";
static Pixmap bwlan_pix[6];
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
 * @param signal Signal received.
 */
static void sigchld_handler(int signal) {
	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}
}

/**
 * @param argc Number of arguments.
 * @param argv Argument list.
 */
int main(int argc, char *argv[]) {
	struct sigaction act;
	int opt;

	while ((opt = getopt(argc, argv, "l:hr:w:")) != -1) {
		switch (opt) {
		case 'l':
			lba_cmd = optarg;
			break;

		case 'h':
			wm_usage(0, usage);

		case 'r':
			rba_cmd = optarg;
			break;

		case 'w':
			wlan = optarg;
			break;

		default:
			wm_usage(1, usage);
		}
	}

	wm_init_dock("wmwireless", WM_STD_DOCK_WINDOW_SIZE, INNERW, &cmds);

	wm_install_alarm_handler(alarm_handler);

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = sigchld_handler;

	if (sigaction(SIGCHLD, &act, 0)) {
		wm_error("Cannot install SIGCHLD handler");
	}

	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bwlan0_xpm, &bwlan_pix[0], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bwlan1_xpm, &bwlan_pix[1], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bwlan2_xpm, &bwlan_pix[2], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bwlan3_xpm, &bwlan_pix[3], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bwlan4_xpm, &bwlan_pix[4], NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), bwlan5_xpm, &bwlan_pix[5], NULL, NULL);

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
	XSetWindowBackgroundPixmap(display, dock_icon_window, bwlan_pix[get_wireless_status()]);

	XClearWindow(display, dock_icon_window);
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
 * Left button press handler.
 *
 * @param x X coordinate of mouse press.
 * @param y Y coordinate of mouse press.
 */
static void left_button_action(int x, int y) {
	wm_spawn(lba_cmd);
}

/**
 * Right button press handler.
 *
 * @param x X coordinate of mouse press.
 * @param y Y coordinate of mouse press.
 */
static void right_button_action(int x, int y) {
	wm_spawn(lba_cmd);
}
