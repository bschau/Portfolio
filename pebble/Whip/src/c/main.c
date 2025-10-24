#define MAIN
#include "include.h"

static void init(void);
static void deinit(void);

#define P_RUN_TIME 1
#define P_WALK_TIME (P_RUN_TIME + 1)
#define P_ITERATIONS (P_WALK_TIME +1)

int main(void)
{
	init();
	show_splash_window();
	app_event_loop();
	deinit();
}

static void init(void)
{
	small_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
	large_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
	
	run_time = persist_read_int(P_RUN_TIME);
	if (run_time == 0) {
		run_time = 60;
	}
	
	walk_time = persist_read_int(P_WALK_TIME);
	
	iterations = persist_read_int(P_ITERATIONS);
	if (iterations == 0) {
		iterations = 5;
	}

	walking = gbitmap_create_with_resource(RESOURCE_ID_WALKING);
	running = gbitmap_create_with_resource(RESOURCE_ID_RUNNING);
	walking_home = gbitmap_create_with_resource(RESOURCE_ID_WALKING_HOME);
	running_home = gbitmap_create_with_resource(RESOURCE_ID_RUNNING_HOME);
	paused = gbitmap_create_with_resource(RESOURCE_ID_PAUSED);
}

static void deinit(void)
{
	remove_number_window();

	gbitmap_destroy(paused);
	gbitmap_destroy(running_home);
	gbitmap_destroy(walking_home);
	gbitmap_destroy(running);
	gbitmap_destroy(walking);

	persist_write_int(P_RUN_TIME, run_time);
	persist_write_int(P_WALK_TIME, walk_time);
	persist_write_int(P_ITERATIONS, iterations);
}