#include "include.h"

static void click_config_provider(void *context);
static void window_load(Window *window);
static void build_total_time_layer(Layer *window_layer, GRect bounds);
static void build_activity_time_layer(Layer *window_layer, GRect bounds);
static void build_activity_graphics_layer(Layer *window_layer, GRect bounds);
static void window_appear(Window *window);
static void window_unload(Window *window);
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void set_total_time_string(void);
static void set_activity_time_string(void);
static void set_activity_graphics(void);
static void redraw_window(void);
static void timer_callback(void *data);
static void handle_timer_callback(void);

static const uint32_t const vibes[] = { 200, 100, 300, 100, 200, 100, 300, 100, 200, 100, 300, 100, 100, 100, 100, 100 };
static VibePattern end_vibes = { .durations = vibes, .num_segments = ARRAY_LENGTH(vibes), };
static Window *window;
static TextLayer *total_time_layer;
static TextLayer *activity_time_layer;
static BitmapLayer *activity_graphics_layer;
static char total_time_buffer[8];
static char activity_time_buffer[8];
static AppTimer *app_timer;
static short total_time, activity_time, activity;
static bool is_paused;

void view_run_init(void)
{
	short unused;

	window_stack_pop_all(false);
	program_start_run();
	program_get_times(&total_time, &unused);
	activity_time = 0;
	activity = 0;
	is_paused = false;
	window = window_new(click_config_provider, window_load, window_unload, window_appear);
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

	build_total_time_layer(window_layer, bounds);
	build_activity_time_layer(window_layer, bounds);
	build_activity_graphics_layer(window_layer, bounds);
}

static void build_total_time_layer(Layer *window_layer, GRect bounds)
{
	total_time_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w / 2, 32 } });
	text_layer_set_font(total_time_layer, small_font);
	text_layer_set_text_alignment(total_time_layer, GTextAlignmentLeft);
	set_total_time_string();
	layer_add_child(window_layer, text_layer_get_layer(total_time_layer));
}

static void build_activity_time_layer(Layer *window_layer, GRect bounds)
{
	activity_time_layer = text_layer_create((GRect) { .origin = { bounds.size.w / 2 , 0 }, .size = { bounds.size.w / 2, 32 } });
	text_layer_set_font(activity_time_layer, small_font);
	text_layer_set_text_alignment(activity_time_layer, GTextAlignmentRight);
	set_activity_time_string();
	layer_add_child(window_layer, text_layer_get_layer(activity_time_layer));
}

static void build_activity_graphics_layer(Layer *window_layer, GRect bounds)
{
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
	if (!program_is_done(activity)) {
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
	if (program_is_run(activity)) {
		if (program_is_turned(activity)) {
			bitmap_layer_set_bitmap(activity_graphics_layer, running_home);
		} else {
			bitmap_layer_set_bitmap(activity_graphics_layer, running);
		}
	} else {
		if (program_is_turned(activity)) {
			bitmap_layer_set_bitmap(activity_graphics_layer, walking_home);
		} else {
			bitmap_layer_set_bitmap(activity_graphics_layer, walking);
		}
	}
}

static void redraw_window(void)
{
	if (activity == 0) {
		return;
	}

	if (program_is_done(activity) || is_paused) {
		if (is_paused) {
			bitmap_layer_set_bitmap(activity_graphics_layer, paused);
		} else {
			bitmap_layer_set_bitmap(activity_graphics_layer, finish);
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
	}

	if (!program_is_done(activity)) {
		app_timer = app_timer_register(1000, timer_callback, NULL);
	}
	redraw_window();
}

static void handle_timer_callback(void)
{
	if (activity_time > 0) {
		activity_time--;
		total_time--;
		if (activity_time < 5) {
			vibes_short_pulse();
		}
		return;
	}

	light_enable_interaction();
	activity = program_next_activity();
	if (program_is_done(activity)) {
		int index, bit;

		app_timer_cancel(app_timer);
		get_day(&index, &bit);
		user_profiles[selected_user].completed[index] |= bit;
		persist_write_data(P_COMPLETED_KEY + selected_user, &user_profiles[selected_user].language, sizeof(struct user_profile)); 
		layer_set_hidden(text_layer_get_layer(total_time_layer), true);
		layer_set_hidden(text_layer_get_layer(activity_time_layer), true);
		vibes_enqueue_custom_pattern(end_vibes);
		return;
	}

	activity_time = program_time_from_activity(activity);
	vibes_long_pulse();
}
