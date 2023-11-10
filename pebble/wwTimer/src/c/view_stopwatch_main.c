#include "include.h"

static void click_config_provider(void *context);
static void window_load(Window *window);
static int build_hour_layer(Layer *window_layer, GRect bounds, int y_pos);
static int build_timer_layer(Layer *window_layer, GRect bounds, int y_pos);
static int build_millis_layer(Layer *window_layer, GRect bounds, int y_pos);
static void build_buttons_layers(Layer *window_layer, GRect bounds);
static void window_appear(Window *window);
static void window_unload(Window *window);
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void down_click_handler(ClickRecognizerRef recognizer, void *context);
static void set_strings(void);
static void redraw_window(void);
static void timer_callback(void *data);

static const uint32_t const add_vibes_data[] = { 100 };
static VibePattern add_vibes = { .durations = add_vibes_data, .num_segments = ARRAY_LENGTH(add_vibes_data), };
static Window *window;
static TextLayer *hour_layer;
static char hour_buffer[8];
static TextLayer *timer_layer;
static char timer_buffer[8];
static TextLayer *millis_layer;
static char millis_buffer[6];
static BitmapLayer *stop_layer;
static BitmapLayer *add_layer;
static BitmapLayer *start_layer;
static AppTimer *app_timer;
static bool running;

void view_stopwatch_main_init(void)
{
	running = false;
	window_stack_pop_all(false);
	window = window_new(click_config_provider, window_load, window_unload, window_appear);
}

static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	int y_pos = 26;

	y_pos += build_hour_layer(window_layer, bounds, y_pos);
	y_pos += build_timer_layer(window_layer, bounds, y_pos);
	y_pos += build_millis_layer(window_layer, bounds, y_pos);

	set_strings();

	build_buttons_layers(window_layer, bounds);
}

static int build_hour_layer(Layer *window_layer, GRect bounds, int y_pos)
{
	hour_layer = text_layer_create((GRect) { .origin = { 0, y_pos }, .size = { bounds.size.w, 21 } });
	text_layer_set_font(hour_layer, small_font);
	text_layer_set_text_alignment(hour_layer, GTextAlignmentCenter);
	text_layer_set_text(hour_layer, hour_buffer);
	layer_add_child(window_layer, text_layer_get_layer(hour_layer));

	return 21;
}

static int build_timer_layer(Layer *window_layer, GRect bounds, int y_pos)
{
	timer_layer = text_layer_create((GRect) { .origin = { 0, y_pos }, .size = { bounds.size.w - 20, 46 } });
	text_layer_set_font(timer_layer, large_light_font);
	text_layer_set_text_alignment(timer_layer, GTextAlignmentCenter);
	text_layer_set_text(timer_layer, timer_buffer);
	layer_add_child(window_layer, text_layer_get_layer(timer_layer));

	return 46;
}

static int build_millis_layer(Layer *window_layer, GRect bounds, int y_pos)
{
	millis_layer = text_layer_create((GRect) { .origin = { 0, y_pos }, .size = { bounds.size.w, 21 } });
	text_layer_set_font(millis_layer, small_font);
	text_layer_set_text_alignment(millis_layer, GTextAlignmentCenter);
	text_layer_set_text(millis_layer, millis_buffer);
	layer_add_child(window_layer, text_layer_get_layer(millis_layer));

	return 21;
}

static void build_buttons_layers(Layer *window_layer, GRect bounds)
{
	stop_layer = bitmap_layer_create((GRect) { .origin = { 124, 2 }, .size = { 18, 18 } });
	layer_add_child(window_layer, bitmap_layer_get_layer(stop_layer));

	start_layer = bitmap_layer_create((GRect) { .origin = { 124, 128 }, .size = { 18, 18 } });
	bitmap_layer_set_bitmap(start_layer, start_button);
	layer_add_child(window_layer, bitmap_layer_get_layer(start_layer));

	add_layer = bitmap_layer_create((GRect) { .origin = { 124, 64 }, .size = { 18, 18 } });
	layer_add_child(window_layer, bitmap_layer_get_layer(add_layer));
}

static void window_appear(Window *window)
{
	redraw_window();
}

static void window_unload(Window *window)
{
	if (app_timer) {
		app_timer_cancel(app_timer);
		app_timer = NULL;
	}
	text_layer_destroy(hour_layer);
	text_layer_destroy(timer_layer);
	text_layer_destroy(millis_layer);
	bitmap_layer_destroy(add_layer);
	bitmap_layer_destroy(start_layer);
	bitmap_layer_destroy(stop_layer);
	window_destroy(window);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (!running) {
		return;
	}

	if (checkpoint_index > (MAX_CHECKPOINTS - 2)) {
		return;
	}

	get_current_time(&checkpoints[checkpoint_index].time, &checkpoints[checkpoint_index].millis);
	checkpoint_index++;
	vibes_enqueue_custom_pattern(add_vibes);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (!running) {
		return;
	}

	app_timer_cancel(app_timer);
	if (checkpoint_index < MAX_CHECKPOINTS) {
		get_current_time(&checkpoints[checkpoint_index].time, &checkpoints[checkpoint_index].millis);
		checkpoint_index++;
	}

	running = false;
	set_strings();
	bitmap_layer_set_bitmap(add_layer, NULL);
	bitmap_layer_set_bitmap(stop_layer, NULL);
	bitmap_layer_set_bitmap(start_layer, start_button);
	vibes_enqueue_custom_pattern(add_vibes);
	view_checkpoints_init();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (running) {
		return;
	}

	running = true;
	bitmap_layer_set_bitmap(add_layer, add_button);
	bitmap_layer_set_bitmap(stop_layer, stop_button);
	bitmap_layer_set_bitmap(start_layer, NULL);
	time_ms(&start_time, &start_millis);
	checkpoint_index = 0;
	app_timer = app_timer_register(0, timer_callback, NULL);
}

static void set_strings(void)
{
	if (running) {
		time_t time;
		uint16_t millis;
		struct tm *tm;

		get_current_time(&time, &millis);
		tm = gmtime(&time);
		snprintf(hour_buffer, sizeof(hour_buffer), "%d:", tm->tm_hour);
		snprintf(timer_buffer, sizeof(timer_buffer), "%02d:%02d", tm->tm_min, tm->tm_sec);
		snprintf(millis_buffer, sizeof(millis_buffer), ".%03i", millis);
	} else {
		strncpy(hour_buffer, "0:", sizeof(hour_buffer));
		strncpy(timer_buffer, "00:00", sizeof(timer_buffer));
		strncpy(millis_buffer, ".000", sizeof(millis_buffer));
	}
}

static void redraw_window(void)
{
	set_strings();
	layer_mark_dirty(text_layer_get_layer(hour_layer));
	layer_mark_dirty(text_layer_get_layer(timer_layer));
	layer_mark_dirty(text_layer_get_layer(millis_layer));
}

static void timer_callback(void *data)
{
	redraw_window();
	app_timer = app_timer_register(0, timer_callback, NULL);
}
