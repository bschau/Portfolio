#ifndef INCLUDE_H
#define INCLUDE_H 1
#include <pebble.h>

struct Checkpoint
{
	time_t time;
	uint16_t millis;
};

#define MAX_CHECKPOINTS 128

/* utils.c */
Window *window_new(void (*click_config_provider)(void *context), void (*load)(Window *window), void (*unload)(Window *window), void (*appear)(Window *window));
void get_current_time(time_t *d_time, uint16_t *d_millis);

/* view_checkpoints.c */
void view_checkpoints_init(void);

/* view_countdown.c */
void view_countdown_init(int minutes_countdown, int seconds_countdown);

/* view_eggtimer_main.c */
void view_eggtimer_main_init(void);

/* view_stopwatch_main.c */
void view_stopwatch_main_init(void);

/* view_main.c */
void view_main_init(void);

#ifdef MAIN
	GFont tiny_font, small_font, large_bold_font, large_light_font;
	GBitmap *down_button, *up_button, *add_button, *start_button;
	GBitmap *stop_button, *pause_button, *eggtimer_icon, *stopwatch_icon;
	struct Checkpoint checkpoints[MAX_CHECKPOINTS];
	int checkpoint_index;
	time_t start_time;
	uint16_t start_millis;
#else
	extern GFont tiny_font, small_font, large_bold_font, large_light_font;
	extern GBitmap *down_button, *up_button, *add_button, *start_button;
	extern GBitmap *stop_button, *pause_button, *eggtimer_icon;
	extern GBitmap *stopwatch_icon;
	extern struct Checkpoint checkpoints[MAX_CHECKPOINTS];
	extern int checkpoint_index;
	extern time_t start_time;
	extern uint16_t start_millis;
#endif

#endif
