#include "include.h"

Window *window_new(void (*click_config_provider)(void *context), void (*load)(Window *window), void (*unload)(Window *window), void (*appear)(Window *window))
{
	Window *window = window_create();

	if (click_config_provider != NULL) {
		window_set_click_config_provider(window, click_config_provider);
	}

	window_set_window_handlers(window, (WindowHandlers) {
		.load = load,
		.unload = unload,
		.appear = appear,
	});
	const bool animated = true;
	window_stack_push(window, animated);
	return window;
}

void get_current_time(time_t *d_time, uint16_t *d_millis)
{
	time_t time;
	uint16_t millis;
	int adjusted_millis;

	time_ms(&time, &millis);
	time -= start_time;
	adjusted_millis = ((int)millis) - start_millis;
	if (adjusted_millis < 0) {
		*d_time = time - 1;
		*d_millis = adjusted_millis + 1000;
	} else {
		*d_time = time;
		*d_millis = adjusted_millis;
	}
}
