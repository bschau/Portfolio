#include "include.h"

static void click_config_provider(void *context);
static void window_load(Window *window);
static void window_appear(Window *window);
static void window_unload(Window *window);
static void timer_callback(void *unused);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);

static Window *window;
static TextLayer *timer_layer;
static TextLayer *paused_layer;
static BitmapLayer *pause_layer;
static char timer_buffer[8];
static const char *paused_text = "Paused";
static int seconds, paused;
static AppTimer *app_timer;

void view_countdown_init(int minutes_countdown, int seconds_countdown)
{
	seconds = (minutes_countdown * 60) + seconds_countdown;
	paused = 0;
	window = window_new(click_config_provider, window_load, window_unload, window_appear);
}

static void click_config_provider(void *context)
{
        window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	pause_layer = bitmap_layer_create((GRect) { .origin = { 124, 2 }, .size = { 18, 18 } });
	bitmap_layer_set_bitmap(pause_layer, pause_button);
	layer_add_child(window_layer, bitmap_layer_get_layer(pause_layer));

	timer_layer = text_layer_create((GRect) { .origin = { 0, 38 }, .size = { bounds.size.w, 50 } });
	text_layer_set_font(timer_layer, large_bold_font);
	text_layer_set_text_alignment(timer_layer, GTextAlignmentCenter);
	text_layer_set_text(timer_layer, timer_buffer);
	snprintf(timer_buffer, sizeof(timer_buffer), "%02i:%02i", seconds / 60, seconds % 60);
	layer_add_child(window_layer, text_layer_get_layer(timer_layer));

	paused_layer = text_layer_create((GRect) { .origin = { 0, 100 }, .size = { bounds.size.w, 24 } });
	text_layer_set_font(paused_layer, small_font);
	text_layer_set_text_alignment(paused_layer, GTextAlignmentCenter);
	text_layer_set_text(paused_layer, paused_text);
	layer_set_hidden(text_layer_get_layer(paused_layer), true);
	layer_add_child(window_layer, text_layer_get_layer(paused_layer));
}

static void window_appear(Window *window)
{
	if (app_timer) {
		app_timer_cancel(app_timer);
	}

	app_timer = app_timer_register(1000, timer_callback, NULL);
}

static void window_unload(Window *window)
{
	if (app_timer) {
		app_timer_cancel(app_timer);
		app_timer = NULL;
	}

	text_layer_destroy(timer_layer);
	bitmap_layer_destroy(pause_layer);
	window_destroy(window);
}

static void timer_callback(void *unused)
{
	app_timer = app_timer_register(1000, timer_callback, NULL);
	if (paused) {
		return;
	}

	seconds--;
	if (seconds > 0) {
		snprintf(timer_buffer, sizeof(timer_buffer), "%02i:%02i", seconds / 60, seconds % 60);
		if (seconds < 2) {
			vibes_short_pulse();
		} else if (seconds < 3) {
			vibes_double_pulse();
		}
	} else {
		app_timer_cancel(app_timer);
		app_timer = NULL;
		strncpy(timer_buffer, "00:00", sizeof(timer_buffer));
		vibes_long_pulse();
	}

	layer_mark_dirty(text_layer_get_layer(timer_layer));
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
	paused = !paused;
	layer_set_hidden(text_layer_get_layer(paused_layer), !paused);
}
