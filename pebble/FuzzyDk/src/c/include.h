#ifndef INCLUDE_H
#define INCLUDE_H
#include <pebble.h>

#ifdef PBL_COLOR
#define BACKGROUND_COLOR (GColorWhite)
#define TEXT_COLOR (GColorBlack)
#else
#define BACKGROUND_COLOR (GColorBlack)
#define TEXT_COLOR (GColorWhite)
#define HIGHLIGHT_COLOR (TEXT_COLOR)
#endif

/* main_window.c */
void main_window_init(void);
void main_window_deinit(void);

/* info_window.c */
void info_window_init(void);
void info_window_deinit(void);
void info_window_update(struct tm *tick_time);

#ifdef MAIN
	Window *main_window, *info_window;
#else
	extern Window *main_window, *info_window;
#endif

#endif
