#include "include.h"

static void show_number_window(char *label, int32_t min, int32_t max, int32_t step, int32_t initial);
static void confirm_number(struct NumberWindow *number_window, void *context);

static char *run_time_label = "Run-time (secs)";
static char *walk_time_label = "Walk-time (secs)";
static char *iterations_label = "Iterations";
static NumberWindow *number_window;
static int number_mode;

void show_configuration_window(void)
{
	number_mode = 0;
	show_number_window(run_time_label, 30, 3600, 30, run_time);
}

static void show_number_window(char *label, int32_t min, int32_t max, int32_t step, int32_t initial)
{
	remove_number_window();
	
	number_window = number_window_create(label, (NumberWindowCallbacks) { .incremented = NULL, .decremented = NULL, .selected = confirm_number}, NULL);
	number_window_set_min(number_window, min);
	number_window_set_max(number_window, max);
	number_window_set_step_size(number_window, step);
	number_window_set_value(number_window, initial);
	window_stack_push((Window *)number_window, true);
}

static void confirm_number(struct NumberWindow *number_window, void *context)
{
	if (number_mode == 0) {
		run_time = number_window_get_value(number_window);
		show_number_window(walk_time_label, 0, 3600, 15, walk_time);
	} else if (number_mode == 1) {
		walk_time = number_window_get_value(number_window);
		show_number_window(iterations_label, 1, 60, 1, iterations);
	} else if (number_mode == 2) {
		iterations = number_window_get_value(number_window);
		remove_number_window();
		show_run_window();
	}
	number_mode++;
}

void remove_number_window(void)
{
	if (number_window == NULL) {
		return;
	}

	window_stack_pop(true);
	number_window_destroy(number_window);
	number_window = NULL;
}