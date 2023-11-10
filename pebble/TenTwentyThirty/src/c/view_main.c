#include "include.h"

static void number_selected_callback(NumberWindow *window, void *context);

static NumberWindow *window;

static char *title = "Rounds:";

void view_main_init(void)
{
	window_stack_pop_all(false);
	if (window == NULL) {
		window = number_window_create(title, (NumberWindowCallbacks) { .selected = number_selected_callback }, NULL);
	}
	number_window_set_min(window, 1);
	number_window_set_max(window, 10);
	number_window_set_step_size(window, 1);
	number_window_set_value(window, rounds_count);
	const bool animated = true;
	window_stack_push((Window *)window, animated);
}

static void number_selected_callback(NumberWindow *window, void *context)
{
	rounds_count = number_window_get_value(window);
	view_action_init();
}
