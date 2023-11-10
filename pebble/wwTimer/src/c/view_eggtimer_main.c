#include "include.h"

static void click_config_provider(void *context);
static void window_load(Window *window);
static void minutes_layer_update(struct Layer *layer, GContext *context);
static void seconds_layer_update(struct Layer *layer, GContext *context);
static void update_timer_layer(struct Layer *layer, GContext *context, char *buffer, int owner_focus);
static void colon_layer_update(struct Layer *layer, GContext *context);
static void build_timer_layers(Layer *window_layer);
static void window_appear(Window *window);
static void window_unload(Window *window);
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void down_click_handler(ClickRecognizerRef recognizer, void *context);

#define P_MINUTES 0
#define P_SECONDS (P_MINUTES + 1)
#define FocusMinutesLayer 0
#define FocusSecondsLayer (FocusMinutesLayer + 1)

static Window *window;
static Layer *minutes_layer;
static char minutes_buffer[4];
static Layer *seconds_layer;
static char seconds_buffer[4];
static Layer *colon_layer;
static int focus;
static int minutes;
static int seconds;

void view_eggtimer_main_init(void)
{
	window_stack_pop_all(false);
	seconds = persist_read_int(P_SECONDS);
	minutes = persist_read_int(P_MINUTES);
	window = window_new(click_config_provider, window_load, window_unload, window_appear);
}

static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_UP, 300, up_click_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 300, down_click_handler);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);

	build_timer_layers(window_layer);
}

static void minutes_layer_update(struct Layer *layer, GContext *context)
{
	snprintf(minutes_buffer, sizeof(minutes_buffer), "%02d", minutes);
	update_timer_layer(layer, context, minutes_buffer, FocusMinutesLayer);
}

static void seconds_layer_update(struct Layer *layer, GContext *context)
{
	snprintf(seconds_buffer, sizeof(seconds_buffer), "%02d", seconds);
	update_timer_layer(layer, context, seconds_buffer, FocusSecondsLayer);
}

static void update_timer_layer(struct Layer *layer, GContext *context, char *buffer, int owner_focus)
{
	GRect rect = layer_get_bounds(layer);
	GRect temporary;

	graphics_context_set_fill_color(context, GColorWhite);
	graphics_fill_rect(context, rect, 0, GCornerNone);

	if (owner_focus == focus) {
		temporary = GRect(rect.origin.x + ((rect.size.w - 19) / 2), rect.origin.y, 19, 10);
		graphics_draw_bitmap_in_rect(context, up_button, temporary);
		temporary = GRect(rect.origin.x + ((rect.size.w - 19) / 2), rect.origin.y + rect.size.h - 10, 19, 10);
		graphics_draw_bitmap_in_rect(context, down_button, temporary);
			
		temporary = GRect(rect.origin.x + 2, rect.origin.y, rect.size.w - 2, rect.size.h);
		temporary.origin.y += (rect.size.h - 50) / 2;
		temporary.size.h = 50;
		graphics_context_set_fill_color(context, GColorBlack);
		graphics_fill_rect(context, temporary, 8, GCornersAll);

		graphics_context_set_text_color(context, GColorWhite);
	} else {
		graphics_context_set_text_color(context, GColorBlack);
	}

	temporary = GRect(rect.origin.x, rect.origin.y, rect.size.w, rect.size.h);
	temporary.origin.y += (rect.size.h - 50) / 2;
	temporary.origin.y -= 2;
	temporary.size.h = 50;

	graphics_draw_text(context, buffer, large_bold_font, temporary, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void colon_layer_update(struct Layer *layer, GContext *context)
{
	GRect rect = layer_get_bounds(layer);
	rect.size.h = 4;

	graphics_context_set_fill_color(context, GColorWhite);
	graphics_fill_rect(context, rect, 0, GCornerNone);

	graphics_context_set_fill_color(context, GColorBlack);
	graphics_fill_rect(context, rect, 2, GCornersAll);

	rect.origin.y += 8;
	graphics_fill_rect(context, rect, 2, GCornersAll);
}

static void build_timer_layers(Layer *window_layer)
{
	minutes_layer = layer_create((GRect) { .origin = { 0, 25 }, .size = { 68, 80 } });
	layer_set_update_proc(minutes_layer, minutes_layer_update);
	layer_add_child(window_layer, minutes_layer);

	seconds_layer = layer_create((GRect) { .origin = { 75, 25 }, .size = { 68, 80 } });
	layer_set_update_proc(seconds_layer, seconds_layer_update);
	layer_add_child(window_layer, seconds_layer);

	colon_layer = layer_create((GRect) { .origin = { 69, 25 + ((80 - 12) / 2) }, .size = { 6, 12 } });
	layer_set_update_proc(colon_layer, colon_layer_update);
	layer_add_child(window_layer, colon_layer);
}

static void window_appear(Window *window)
{
	focus = FocusMinutesLayer;
	layer_mark_dirty(minutes_layer);
	layer_mark_dirty(seconds_layer);
}

static void window_unload(Window *window)
{
	layer_destroy(minutes_layer);
	layer_destroy(seconds_layer);
	layer_destroy(colon_layer);
	window_destroy(window);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
	focus++;
	if (focus > 1) {
		if (minutes == 0 && seconds == 0) {
			focus = FocusMinutesLayer;
		} else {
			persist_write_int(P_MINUTES, minutes);
			persist_write_int(P_SECONDS, seconds);
			view_countdown_init(minutes, seconds);
			return;
		}
	}
	layer_mark_dirty(minutes_layer);
	layer_mark_dirty(seconds_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (focus == FocusMinutesLayer) {
		if (minutes >= 99) {
			minutes = 0;
		} else {
			minutes++;
		}
		layer_mark_dirty(minutes_layer);
	} else {
		if (seconds >= 59) {
			seconds = 0;
		} else {
			seconds++;
		}
		layer_mark_dirty(seconds_layer);
	}
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (focus == FocusMinutesLayer) {
		if (minutes <= 0) {
			minutes = 99;
		} else {
			minutes--;
		}
		layer_mark_dirty(minutes_layer);
	} else {
		if (seconds <= 0) {
			seconds = 59;
		} else {
			seconds--;
		}
		layer_mark_dirty(seconds_layer);
	}
}
