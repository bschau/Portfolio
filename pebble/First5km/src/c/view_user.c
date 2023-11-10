#include "include.h"

static void number_selected_callback(NumberWindow *window, void *context);

static NumberWindow *window;

void view_user_init(void)
{
	if (window == NULL) {
		window = number_window_create(locale_get(LOCALE_SELECTUSER), (NumberWindowCallbacks) { .selected = number_selected_callback }, NULL);
	}
	number_window_set_min(window, 1);
	number_window_set_max(window, NUMBER_OF_USERS);
	number_window_set_step_size(window, 1);
	number_window_set_value(window, selected_user + 1);
	const bool animated = true;
	window_stack_push((Window *)window, animated);
}

static void number_selected_callback(NumberWindow *window, void *context)
{
	selected_user = number_window_get_value(window) - 1;
	view_main_init();
}
