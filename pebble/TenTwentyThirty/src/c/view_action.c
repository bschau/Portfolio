#include "include.h"

static void window_load(Window *window);
static void layer_update(struct Layer *layer, GContext *context);
static void build_layer(Layer *window_layer);
static void build_copyright_layer(Layer *window_layer);
static void window_appear(Window *window);
static void window_unload(Window *window);
static void timer_callback(void *unused);

static char *copyright_text = "(c) schau.dk";
static Window *window;
static Layer *layer;
static TextLayer *copyright_layer;
static int step;
static int length;
static int rounds;
static char rounds_buffer[4];
static AppTimer *app_timer;

void view_action_init(void)
{
	rounds = rounds_count;
	snprintf(rounds_buffer, sizeof(rounds_buffer), "%i", rounds);
	step = 0;
	length = 30;
	window = window_new(NULL, window_load, window_unload, window_appear);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);

	build_layer(window_layer);
	build_copyright_layer(window_layer);
}

static void layer_update(struct Layer *layer, GContext *context)
{
	GRect rect = layer_get_bounds(layer);
	GRect temporary = GRect(rect.origin.x, rect.origin.y, rect.size.w, 61);
	
	graphics_draw_bitmap_in_rect(context, step == 0 ? turtle_image : step == 1 ? rabbit_image : leopard_image , temporary);

	graphics_context_set_text_color(context, GColorBlack);
	rect.origin.y = rect.origin.y + (rect.size.h - 44);
	rect.size.h = rect.size.h - 44;
	graphics_draw_text(context, rounds_buffer, large_font, rect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void build_layer(Layer *window_layer)
{
	layer = layer_create((GRect) { .origin = { 2, 15 }, .size = { 140, 110 } });
	layer_set_update_proc(layer, layer_update);
	layer_add_child(window_layer, layer);
}

static void build_copyright_layer(Layer *window_layer)
{
	GRect bounds = layer_get_bounds(window_layer);
	copyright_layer = text_layer_create((GRect) { .origin = { 0, 130 }, .size = { bounds.size.w, 14 } });
	text_layer_set_font(copyright_layer, tiny_font);
	text_layer_set_text_alignment(copyright_layer, GTextAlignmentCenter);
	text_layer_set_text(copyright_layer, copyright_text);
	layer_add_child(window_layer, text_layer_get_layer(copyright_layer));
}

static void window_appear(Window *window)
{
	layer_mark_dirty(layer);
	app_timer_register(1000, timer_callback, NULL);
}

static void window_unload(Window *window)
{
	if (app_timer) {
		app_timer_cancel(app_timer);
		app_timer = NULL;
	}

	layer_destroy(layer);
	text_layer_destroy(copyright_layer);
	window_destroy(window);
}

static void timer_callback(void *unused)
{
	app_timer_register(1000, timer_callback, NULL);

	length--;
	if (length == 0) {
		step++;
		if (step == 3) {
			step = 0;
			rounds--;
			if (rounds == 0) {
				vibes_double_pulse();
				view_main_init();
				return;
			}
		}

		if (step == 0) {
			vibes_long_pulse();
		} else {
			vibes_short_pulse();
		}

		length = (step == 0) ? 30 : (step == 1) ? 20 : 10;
	}

	snprintf(rounds_buffer, sizeof(rounds_buffer), "%i", rounds);
	layer_mark_dirty(layer);
}
