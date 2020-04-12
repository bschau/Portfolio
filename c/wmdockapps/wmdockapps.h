#ifndef WMDOCKAPPS_H
#define WMDOCKAPPS_H

/** macros */
#define WM_STD_DOCK_WINDOW_SIZE 64
#define WM_STD_ICON_WINDOW_SIZE 48

/** structs */
typedef struct {
	void (*expose)(void);
	void (*left_button_press)(int, int);
	void (*right_button_press)(int, int);
} wm_cmds;

/** forward decls */
void wm_error(const char *fmt, ...);
void wm_init_dock(const char *program_name, int dock_window_size, int icon_window_size, wm_cmds *cmds);
GC wm_get_gc(void);
void wm_close_dock(void);
void wm_install_alarm_handler(void (*handler)(int signal));
void wm_install_sigchld_handler(void (*handler)(int signal));
void wm_event_loop(void);
void wm_usage(int ec, const char *txt);
unsigned long wm_get_black_pixel_colour(void);
void wm_spawn(char *cmd);

#ifdef WMDOCKAPPS
	Display *display;
	Window dock_window, dock_icon_window;
#else
	extern Display *display;
	extern Window dock_window, dock_icon_window;
#endif

#endif
