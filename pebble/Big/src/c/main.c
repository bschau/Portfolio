#include <pebble.h>
#include "InfoLayer.h"

static void init(void);
static BitmapLayer *create_digit_bitmap(Layer *window_layer, int x, int y);
static void deinit(void);
static void window_load(Window *window);
static void window_unload(Window *window);
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed);
static void accel_tap_handler(AccelAxisType axis, int32_t direction);
static int get_battery_index(void);

static GBitmap *digits[10] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static int digit_ids[10] = { RESOURCE_ID_0, RESOURCE_ID_1, RESOURCE_ID_2, RESOURCE_ID_3, RESOURCE_ID_4,
			     RESOURCE_ID_5, RESOURCE_ID_6, RESOURCE_ID_7, RESOURCE_ID_8, RESOURCE_ID_9 };
static char time_text[] = "0000";
static BitmapLayer *ul, *ur, *ll, *lr;
static InfoLayer *info_layer;
static Window *window;
static char time_format[6];

int main(void)
{
	init();
	app_event_loop();
	deinit();
}

static void init(void)
{
	if (clock_is_24h_style()) {
		strncpy(time_format, "%H%M", sizeof(time_format));
	} else {
		strncpy(time_format, "%I%M", sizeof(time_format));
	}

	for (int i = 0; i < 10; i++) {
		digits[i] = gbitmap_create_with_resource(digit_ids[i]);
	}

	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	Layer *window_layer = window_get_root_layer(window);
	GRect unobstructed_area = layer_get_unobstructed_bounds(window_layer);
	int x_offset = (unobstructed_area.size.w - 142) / 2;
	int y_offset = (unobstructed_area.size.h - 168) / 2;
	
	ul = create_digit_bitmap(window_layer, 2 + x_offset, y_offset);
	ur = create_digit_bitmap(window_layer, 74 + x_offset, y_offset);
	ll = create_digit_bitmap(window_layer, 2 + x_offset, 85 + y_offset);
	lr = create_digit_bitmap(window_layer, 74 + x_offset, 85 + y_offset);

	info_layer = info_layer_create(unobstructed_area.size.w, unobstructed_area.size.h);
	layer_add_child(window_layer, info_layer_get_layer(info_layer));

	window_stack_push(window, true);
}

static BitmapLayer *create_digit_bitmap(Layer *window_layer, int x, int y)
{
	BitmapLayer *bitmap_layer = bitmap_layer_create(GRect(x, y, 67, 83));
	bitmap_layer_set_background_color(bitmap_layer, GColorWhite);
	layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
	return bitmap_layer;
}

static void deinit(void)
{
	info_layer_destroy(info_layer);

	bitmap_layer_destroy(ul);
	bitmap_layer_destroy(ur);
	bitmap_layer_destroy(ll);
	bitmap_layer_destroy(lr);

	window_destroy(window);

	for (int i = 0; i < 10; i++) {
		gbitmap_destroy(digits[i]);
	}
}

static void window_load(Window *window)
{
	tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);
	accel_tap_service_subscribe(&accel_tap_handler);
}

static void window_unload(Window *window)
{
	info_layer_hide(info_layer);
	accel_tap_service_unsubscribe();
	tick_timer_service_unsubscribe();
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{
	if ((units_changed & SECOND_UNIT) == SECOND_UNIT) {
		tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	}

	strftime(time_text, sizeof(time_text), time_format, tick_time);
	bitmap_layer_set_bitmap(ul, digits[time_text[0] - '0']);
	bitmap_layer_set_bitmap(ur, digits[time_text[1] - '0']);
	bitmap_layer_set_bitmap(ll, digits[time_text[2] - '0']);
	bitmap_layer_set_bitmap(lr, digits[time_text[3] - '0']);

	info_layer_set_info(info_layer, tick_time, get_battery_index());
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction)
{
	info_layer_show(info_layer);
}

static int get_battery_index(void)
{
	BatteryChargeState charge_state = battery_state_service_peek();
	int index = charge_state.charge_percent / 10;
	if (index < 0) {
		index = 0;
	} else if (index > 9) {
		index = 9;
	}

	return index;
}