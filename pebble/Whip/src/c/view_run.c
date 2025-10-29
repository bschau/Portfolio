#include "include.h"

static void click_config_provider(void *context);
static void window_load(Window *window);
static void window_appear(Window *window);
static void window_unload(Window *window);
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void set_total_time_string(void);
static void set_activity_time_string(void);
static void set_activity_graphics(void);
static void redraw_window(void);
static void timer_callback(void *data);
static void handle_timer_callback(void);

static Window *window;
static TextLayer *total_time_layer;
static TextLayer *activity_time_layer;
static BitmapLayer *activity_graphics_layer;
static char total_time_buffer[8];
static char activity_time_buffer[8];
static AppTimer *app_timer;
static short total_time, activity_time, activity, turn_time;
static bool is_paused;

void show_run_window(void)
{
	activity_time = run_time;
	is_paused = false;
	activity = -1;
	
	total_time = (run_time + walk_time) * iterations;
	turn_time = (total_time / 2) + 1;
	
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);

	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
		.appear = window_appear,
	});
	window_stack_push(window, true);
	
	app_timer = app_timer_register(10, timer_callback, NULL);
}

static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	total_time_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w / 2, 32 } });
	text_layer_set_font(total_time_layer, small_font);
	text_layer_set_text_alignment(total_time_layer, GTextAlignmentLeft);
	set_total_time_string();
	layer_add_child(window_layer, text_layer_get_layer(total_time_layer));

	activity_time_layer = text_layer_create((GRect) { .origin = { bounds.size.w / 2 , 0 }, .size = { bounds.size.w / 2, 32 } });
	text_layer_set_font(activity_time_layer, small_font);
	text_layer_set_text_alignment(activity_time_layer, GTextAlignmentRight);
	set_activity_time_string();
	layer_add_child(window_layer, text_layer_get_layer(activity_time_layer));

	activity_graphics_layer = bitmap_layer_create((GRect) { .origin = { (bounds.size.w - 120) / 2, bounds.size.h - 121 }, .size = { 120, 120 } });
	set_activity_graphics();
	layer_add_child(window_layer, bitmap_layer_get_layer(activity_graphics_layer));
}

static void window_appear(Window *window)
{
	redraw_window();
}

static void window_unload(Window *window)
{
	app_timer_cancel(app_timer);
	text_layer_destroy(total_time_layer);
	text_layer_destroy(activity_time_layer);
	bitmap_layer_destroy(activity_graphics_layer);
	window_destroy(window);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (total_time > -1) {
		is_paused = !is_paused;
	}
}

static void set_total_time_string(void)
{
	snprintf(total_time_buffer, sizeof(total_time_buffer), "%i:%02i", total_time / 60 , total_time % 60);
	text_layer_set_text(total_time_layer, total_time_buffer);
}

static void set_activity_time_string(void)
{
	snprintf(activity_time_buffer, sizeof(activity_time_buffer), "%i:%02i", activity_time / 60 , activity_time % 60);
	text_layer_set_text(activity_time_layer, activity_time_buffer);
}

static void set_activity_graphics(void)
{
	if (activity == -1) {
		return;
	}

	if (activity % 2 == 0) {
		if (total_time < turn_time) {
			bitmap_layer_set_bitmap(activity_graphics_layer, running_home);
		} else {
			bitmap_layer_set_bitmap(activity_graphics_layer, running);
		}
	} else {
		if (total_time < turn_time) {
			bitmap_layer_set_bitmap(activity_graphics_layer, walking_home);
		} else {
			bitmap_layer_set_bitmap(activity_graphics_layer, walking);
		}
	}
}

static void redraw_window(void)
{
	if (total_time == -1 || is_paused) {
		if (is_paused) {
			bitmap_layer_set_bitmap(activity_graphics_layer, paused);
		}
	} else {
		set_total_time_string();
		set_activity_time_string();
		set_activity_graphics();
	}

	layer_mark_dirty(text_layer_get_layer(total_time_layer));
	layer_mark_dirty(text_layer_get_layer(activity_time_layer));
	layer_mark_dirty(bitmap_layer_get_layer(activity_graphics_layer));
}

static void timer_callback(void *data)
{
	if (!is_paused) {
		handle_timer_callback();

		if (total_time == -1) {
			window_stack_pop(true);
			show_finish_window();
			return;
		}
	}

	redraw_window();
	
	if (total_time > -1) {
		app_timer = app_timer_register(1000, timer_callback, NULL);
	}
}

static void handle_timer_callback(void)
{
	if (activity == -1) {
		activity = 0;
		return;
	}

	total_time--;
	if (activity_time > 1) {
		activity_time--;
		if (activity_time < 3) {
			vibes_short_pulse();
		}
		return;
	}

	light_enable_interaction();
	if (total_time < 1) {
		total_time = -1;
		app_timer_cancel(app_timer);
		app_timer = NULL;
		return;
	}

	activity = (activity + 1) % 2;
	if (walk_time > 0) {
		activity_time = (activity == 0) ? run_time : walk_time;
	} else {
		activity_time = run_time;
		activity = 0;
	}

	vibes_long_pulse();
}