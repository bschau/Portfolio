#ifndef INCLUDE_H
#define INCLUDE_H

#include <pebble.h>

/* view_configuration.c */
void show_configuration_window(void);
void remove_number_window(void);

/* view_finish.c */
void show_finish_window(void);

/* view_run.c */
void show_run_window(void);

/* view_splash.c */
void show_splash_window(void);

#ifdef MAIN
	int run_time, walk_time, iterations;
	GFont small_font, large_font;
	GBitmap *running, *walking, *running_home, *walking_home, *paused;
#else
	extern int run_time, walk_time, iterations;
	extern GFont small_font, large_font;
	extern GBitmap *running, *walking, *running_home, *walking_home, *paused;
#endif

#endif