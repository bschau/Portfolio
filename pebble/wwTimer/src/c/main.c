#define MAIN
#include "include.h"

static void init(void);
static void deinit(void);

int main(void)
{
	init();
	view_main_init();
	app_event_loop();
	deinit();
}

static void init(void)
{
	tiny_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
	small_font = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
	large_bold_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
	large_light_font = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
	down_button = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOWN_BUTTON);
	up_button = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UP_BUTTON);
	add_button = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ADD_BUTTON);
	start_button = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_START_BUTTON);
	stop_button = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STOP_BUTTON);
	pause_button = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAUSE_BUTTON);
	eggtimer_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EGGTIMER);
	stopwatch_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STOPWATCH);
}

static void deinit(void)
{
	gbitmap_destroy(up_button);
	gbitmap_destroy(down_button);
	gbitmap_destroy(pause_button);
	gbitmap_destroy(stop_button);
	gbitmap_destroy(start_button);
	gbitmap_destroy(add_button);
	gbitmap_destroy(eggtimer_icon);
	gbitmap_destroy(stopwatch_icon);
}
