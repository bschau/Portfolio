#include "include.h"

static void window_load(Window *window);
static void window_appear(Window *window);
static void window_unload(Window *window);

static const uint32_t vibes[] = { 200, 100, 300, 100, 200, 100, 300, 100, 200, 100, 300, 100, 100, 100, 100, 100 };
static VibePattern end_vibes = { .durations = vibes, .num_segments = ARRAY_LENGTH(vibes), };
static Window *window;
static BitmapLayer *finish_layer;
static GBitmap *finish_bitmap;

void show_finish_window(void)
{
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.unload = window_unload
	});
	window_stack_push(window, true);
	vibes_enqueue_custom_pattern(end_vibes);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	finish_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FINISH);
	finish_layer = bitmap_layer_create((GRect) { .origin = { (bounds.size.w - 120) / 2, (bounds.size.h - 120) / 2 }, .size = { 120, 120 } });
	bitmap_layer_set_bitmap(finish_layer, finish_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(finish_layer));
}

static void window_appear(Window *window)
{
	layer_mark_dirty(bitmap_layer_get_layer(finish_layer));
}

static void window_unload(Window *window)
{
	bitmap_layer_destroy(finish_layer);
	gbitmap_destroy(finish_bitmap);
	window_destroy(window);
}