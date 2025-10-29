#include "include.h"

static void click_config_provider(void *context);
static void window_load(Window *window);
static void window_appear(Window *window);
static void window_unload(Window *window);
static void cancel_timer(void);
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void timer_callback(void *data);

static char *title_text = "Whip v1";
static char *copy_text = "(c) Brian Schau";
static Window *window;
static AppTimer *app_timer;
static TextLayer *title_layer;
static TextLayer *copy_layer;
static BitmapLayer *splash_layer;
static GBitmap *splash_bitmap;

void show_splash_window(void)
{
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);

	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.unload = window_unload
	});
	window_stack_push(window, true);

	app_timer = app_timer_register(5000, timer_callback, NULL);
}

static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, select_click_handler);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	title_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 36 } });
	text_layer_set_font(title_layer, large_font);
	text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
	text_layer_set_text(title_layer, title_text);
	layer_add_child(window_layer, text_layer_get_layer(title_layer));

	copy_layer = text_layer_create((GRect) { .origin = { 0, 130 }, .size = { bounds.size.w, 30 } });
	text_layer_set_font(copy_layer, small_font);
	text_layer_set_text_alignment(copy_layer, GTextAlignmentCenter);
	text_layer_set_text(copy_layer, copy_text);
	layer_add_child(window_layer, text_layer_get_layer(copy_layer));

	splash_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SPLASH);
	splash_layer = bitmap_layer_create((GRect) { .origin = { (bounds.size.w - 72) / 2, (bounds.size.h - 72) / 2 }, .size = { 72, 72 } });
	bitmap_layer_set_bitmap(splash_layer, splash_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(splash_layer));
}

static void window_appear(Window *window)
{
	layer_mark_dirty(text_layer_get_layer(title_layer));
	layer_mark_dirty(text_layer_get_layer(copy_layer));
	layer_mark_dirty(bitmap_layer_get_layer(splash_layer));
}

static void window_unload(Window *window)
{
	cancel_timer();

	bitmap_layer_destroy(splash_layer);
	gbitmap_destroy(splash_bitmap);
	text_layer_destroy(copy_layer);
	text_layer_destroy(title_layer);
	window_destroy(window);
}

static void cancel_timer(void)
{
	if (app_timer == NULL) {
		return;
	}
	
	app_timer_cancel(app_timer);
	app_timer = NULL;
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
	timer_callback(NULL);
}

static void timer_callback(void *data)
{
	cancel_timer();
	window_stack_pop(true);
	show_configuration_window();
}