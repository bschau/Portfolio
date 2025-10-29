#ifndef INCLUDE_H
#define INCLUDE_H 1
#include <pebble.h>
#include "locale.h"

#define MAX_DAYS 48
#define NUMBER_OF_USERS 8
#define P_VERSION_KEY 0
#define P_MULTIUSER_KEY (P_VERSION_KEY + 1)
#define P_SELECTEDUSER_KEY (P_MULTIUSER_KEY +1)
#define P_COMPLETED_KEY 100

struct user_profile {
	unsigned char language;
	unsigned char completed[MAX_DAYS / 8];
};

/* locale.c */
char *locale_get(int key);

/* program.c */
void program_select_day(short day);
void program_get_times(short *total_time, short *run_time);
void program_start_run(void);
short program_next_activity(void);
void program_get_internals(short *activities, short *run_index, short *day_index);
short program_time_from_activity(short activity);
bool program_is_run(short activity);
bool program_is_walk(short activity);
bool program_is_turned(short activity);
bool program_is_done(short activity);

/* utils.c */
Window *window_new(void (*click_config_provider)(void *context), void (*load)(Window *window), void (*unload)(Window *window), void (*appear)(Window *window));
void get_day(int *index, int *bit);

/* view_main.c */
void view_main_init(void);

/* view_main_menu.c */
void view_main_menu_init(void);

/* view_run.c */
void view_run_init(void);

/* view_user.c */
void view_user_init(void);

#ifdef MAIN
	GFont small_font, large_font;
	int day;
	GBitmap *running, *walking, *running_home, *walking_home, *finish, *paused;

	/* These must be persisted */
	int multi_user = 0, selected_user = 0;
	struct user_profile user_profiles[NUMBER_OF_USERS];
#else
	extern GFont small_font, large_font;
	extern int day, multi_user, selected_user;
	extern struct user_profile user_profiles[NUMBER_OF_USERS];
	extern GBitmap *running, *walking, *running_home, *walking_home, *finish, *paused;
#endif

#endif
