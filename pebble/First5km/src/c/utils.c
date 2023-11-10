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

void get_day(int *index, int *bit)
{
	const int stride = MAX_DAYS / 8;

	*index = (selected_user * stride) + (day / 8);
	*bit = 1 << (day % 8);
}
