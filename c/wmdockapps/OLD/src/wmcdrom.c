/*
	wmcdrom (c) 2010- Brian Schau <brian@schau.dk>

	wmcdrom is a dock application which provides a way to easily
	mount and unmount cdrom filesystems.

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
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <wmdockapps.h>
#include "res/mount.xpm"
#include "res/unmount.xpm"

/* macros */
#define MOUNTCMD "/bin/mount"
#define UMOUNTCMD "/bin/umount"
#define EJECTCMD "/usr/bin/eject"
#define BROWSECMD "/usr/bin/thunar /mnt/cdrom"
#define MAXLINELEN 128
typedef enum { DeviceNotMounted, DeviceMounted } device_status;

/* protos */
static void alarm_handler(int signal);
static void left_button_press(int x, int y);
static void right_button_press(int x, int y);
static device_status mount_status(void);
static void update_mount_status(void);
static void mount_device(void);
static void unmount_device(char *com);
static char *qualify_device(char *str);

/* globals */
static char *usage = "Usage: wmcdrom [OPTIONS] device\n\n" \
	"[OPTIONS]\n" \
	"  -b browsecmd   Browse command (" BROWSECMD ")\n" \
	"  -e ejectcmd    Eject command (" EJECTCMD ")\n" \
        "  -n             Do not browse after mount\n" \
	"  -h             This page\n" \
	"  -m mountcmd    Mount command (" MOUNTCMD ")\n" \
	"  -u umountcmd   Unmount command (" UMOUNTCMD ")\n";
static wm_cmds cmds = { update_mount_status, left_button_press, right_button_press };
static device_status old_device_status = -1;
static Pixmap mount_pix, unmount_pix;
static char *cmd_eject = EJECTCMD, *cmd_mount = MOUNTCMD;
static char *cmd_umount = UMOUNTCMD, *cmd_browse = BROWSECMD, *device;
static int browse = 1, device_len;
extern int errno;

/**
 * @param signal Signal received.
 */
static void alarm_handler(int signal) {
	update_mount_status();
	alarm(1);
}
	
/**
 * @param argc Number of arguments.
 * @param argv Argument list.
 */
int main(int argc, char *argv[]) {
	int opt;

	while ((opt = getopt(argc, argv, "be:hnm:u:")) != -1) {
		switch (opt) {
		case 'b':
			cmd_browse = optarg;
			break;

		case 'e':
			cmd_eject = optarg;
			break;

		case 'h':
			wm_usage(0, usage);

		case 'n':
			browse = 0;
			break;

		case 'm':
			cmd_mount = optarg;
			break;

		case 'u':
			cmd_umount = optarg;
			break;

		default:
			wm_usage(1, usage);
		}
	}

	if ((argc - optind) == 1) {
		device = argv[optind];
		device_len = strlen(device);
	} else {
		wm_usage(1, usage);
	}

	wm_init_dock("wmcdrom", WM_STD_DOCK_WINDOW_SIZE, WM_STD_ICON_WINDOW_SIZE, &cmds);

	wm_install_alarm_handler(alarm_handler);
	wm_install_sigchld_handler(NULL);

	XpmCreatePixmapFromData(display, DefaultRootWindow(display), mount_xpm, &mount_pix, NULL, NULL);
	XpmCreatePixmapFromData(display, DefaultRootWindow(display), unmount_xpm, &unmount_pix, NULL, NULL);

	update_mount_status();
	alarm(1);
	
	wm_event_loop();

	alarm(0);
	exit(0);
}

/*
 * @return Mount status for this device.
 */
static device_status mount_status(void) {
	FILE *proc_mounts = fopen("/proc/mounts", "r");
	device_status d = DeviceNotMounted;
	char buffer[MAXLINELEN + 2];
	char *s;

	if (proc_mounts == NULL) {
		fprintf(stderr, "Cannot open /proc/mounts (%s)\n", strerror(errno));
		return DeviceNotMounted;
	}

	for (;;) {
		if ((s = fgets(buffer, MAXLINELEN, proc_mounts)) == NULL) {
			break;
		}

		if ((strncmp(s, device, device_len)) == 0) {
			if (*(s + device_len) == ' ') {
				d = DeviceMounted;
				break;
			}
		}
	}

	fclose(proc_mounts);
	return d;
}

/**
 * Update the graphics in the dock_window with the current mount status.
 */
static void update_mount_status(void) {
	device_status d = mount_status();

	if (d == old_device_status) {
		return;
	}

	old_device_status = d;

	if (display) {
		XSetWindowBackgroundPixmap(display, dock_icon_window, (d == DeviceMounted) ? mount_pix : unmount_pix);
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
	if (old_device_status == DeviceNotMounted) {
		mount_device();
	} else {
		unmount_device(cmd_umount);
	}
}

/**
 * Right mouse button handler.
 *
 * @param x X coordinate of mouse press.
 * @param y Y coordinate of mouse press.
 */
static void right_button_press(int x, int y) {
	if (old_device_status == DeviceNotMounted) {
		mount_device();
	} else {
		unmount_device(cmd_eject);
	}
}

/**
 * Mount the media.
 */
static void mount_device(void) {
	char *str = qualify_device(cmd_mount);

	if (str) {
		system(str);
		free(str);

		if (browse) {
			wm_spawn(cmd_browse);
		}
	}
}

/**
 * @param com Command used to unmount (and possibly eject) media.
 */
static void unmount_device(char *com) {
	char *str = qualify_device(com);

	if (str) {
		system(str);
		free(str);
	}
}

/**
 * Add device to end of string.
 *
 * @param str String.
 * @return Qualified path or NULL (failure).
 */
static char *qualify_device(char *str) {
	int len = strlen(str);
	char *d, *p;

	if ((d = (char *) calloc(len + 2 + device_len, 1)) == NULL) {
		fprintf(stderr, "Out of memory ...\n");
		return NULL;
	}

	p = d;
	memmove(p, str, len);
	p += len;
	*p++ = ' ';
	memmove(p, device, device_len);

	return d;
}
