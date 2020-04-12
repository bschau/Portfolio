#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#define WMDOCKAPPS
#include "wmdockapps.h"

/* macros */
#define EVER ;;

/* protos */
static void _sigchld_handler(int signal);

/* globals */
static wm_cmds *_wm_cmds = NULL;
static int _icon_window_size;
static unsigned long _black;

/**
 * @param signal Signal received.
 */
static void _sigchld_handler(int signal) {
	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}
}

/**
 * Error handler.
 *
 * @param fmt Format string.
 * @param ... varargs.
 */
void wm_error(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, "\n");
	exit(1);
}

/**
 * Initialize dock.
 *
 * @param program_name Name of program.
 * @param dock_window_size Size of dock window (64 pixels)
 * @param icon_window_size Size of icon window (typically 48 pixels)
 * @param cmds Commands this dock app listens to.
 */
void wm_init_dock(const char *program_name, int dock_window_size, int icon_window_size, wm_cmds *cmds) {
	char *env_display = getenv("DISPLAY");
	long ev_mask = ExposureMask;
	XClassHint *class_hint;
	XWMHints *hints;

	if (env_display == NULL) {
		env_display = "";
	}

	if ((display = XOpenDisplay(env_display)) == NULL) {
		wm_error("Cannot open display");
	}

	_icon_window_size = icon_window_size;
	_black = BlackPixel(display, DefaultScreen(display));

	dock_window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, dock_window_size, dock_window_size, 0, _black, _black);

	if ((class_hint = XAllocClassHint()) == NULL) {
		wm_error("Failed to allocate class hint");
	}

	class_hint->res_class = (char *) program_name;
	class_hint->res_name = (char *) program_name;
	XSetClassHint(display, dock_window, class_hint);
	XFree(class_hint);

	dock_icon_window = XCreateSimpleWindow(display, dock_window, (dock_window_size - icon_window_size) / 2, (dock_window_size - icon_window_size) / 2, icon_window_size, icon_window_size, 0, _black, _black);

	if ((hints = XAllocWMHints()) == NULL) {
		wm_error("Failed to allocate hints");
	}

	hints->flags = StateHint | WindowGroupHint | IconWindowHint;
	hints->initial_state = WithdrawnState;
	hints->window_group = dock_window;
	hints->icon_window = dock_icon_window;
	hints->icon_x = 0;
	hints->icon_y = 0;
	XSetWMHints(display, dock_window, hints);

	XSelectInput(display, dock_window, ExposureMask);
	if ((cmds->left_button_press) || (cmds->right_button_press)) {
		ev_mask |= ButtonPressMask | ButtonReleaseMask;
	}

	XSelectInput(display, dock_icon_window, ev_mask);
	_wm_cmds = cmds;
}

/**
 * @return graphics context./
 */
GC wm_get_gc(void) {
	GC gc = XCreateGC(display, dock_icon_window, 0, NULL);

	if (!gc) {
		wm_error("Failed to create graphics context");
	}

	return gc;
}

/**
 * Close the dock.
 */
void wm_close_dock(void) {
	if (display) {
		Display *d = display;

		display = NULL;
		XCloseDisplay(d);
	}
}

/**
 * Install alarm signal handler.
 *
 * @param handler Alarm handler.
 */
void wm_install_alarm_handler(void (*handler)(int signal)) {
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL)) {
		wm_error("Cannot install Alarm handler");
	}
}

/**
 * Install sigchld signal handler.
 *
 * @param handler Sigchld handler or NULL to use builtin.
 */
void wm_install_sigchld_handler(void (*handler)(int signal)) {
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = handler ? handler : _sigchld_handler;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGCHLD, &act, NULL)) {
		wm_error("Cannot install SIGCHLD handler");
	}
}

/**
 * Handle events from X server.
 */
void wm_event_loop(void) {
	XEvent ev;

	XMapRaised(display, dock_window);
	XFlush(display);

	for (EVER) {
		XNextEvent(display, &ev);

		switch (ev.type) {
		case Expose:
			if (_wm_cmds->expose) {
				_wm_cmds->expose();
			}
			break;

		case ButtonRelease:
			if ((ev.xbutton.x < 0) || (ev.xbutton.x > _icon_window_size) || (ev.xbutton.y < 0) || (ev.xbutton.y > _icon_window_size)) {
				break;
			}
			
			if (ev.xbutton.button == 1) {
				if (_wm_cmds->left_button_press) {
					_wm_cmds->left_button_press(ev.xbutton.x, ev.xbutton.y);
				}
			} else if (ev.xbutton.button == 3) {
				if (_wm_cmds->right_button_press) {
					_wm_cmds->right_button_press(ev.xbutton.x, ev.xbutton.y);
				}
			}
			break;

		case DestroyNotify:
			wm_close_dock();
			return;
		}
	}
}

/**
 * Write usage text.
 *
 * @þaram ec Exit code.
 * @param txt Usage txt.
 */
void wm_usage(int ec, char *txt) {
	FILE *f = stdout;

	if (ec) {
		f = stderr;
	}

	fprintf(f, txt);
	exit(ec);
}

/**
 * @return Black pixel colour.
 */
unsigned long wm_get_black_pixel_colour(void) {
	return _black;
}

/**
 * @param cmd Command to run.
 */
void wm_spawn(char *cmd) {
	pid_t p;

	if ((p = fork()) == 0) {
		system(cmd);
		exit(0);
	}

	if (p == -1) {
		fprintf(stderr, "Failed to run '%s'\n", cmd);
	}
}
