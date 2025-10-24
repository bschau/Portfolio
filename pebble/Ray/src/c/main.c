#define MAIN
#include "ray.h"

static void init(void);
static void deinit(void);
static void window_load(Window *window);
static void window_unload(Window *window);
static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed);
static void handle_minute_tick(struct tm *tick_time);
static void set_battery_bitmap();

static char time_text[] = "00:00";
static char date_text[] = "Xxxxxxxxx 00";
static BitmapLayer *battery_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;
static char time_format[6];
static GBitmap *batteries[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
static Window *window;

int main(void) {
	init();
	app_event_loop();
	deinit();
}

static void init(void) {
	time_t current_time;
	struct tm *tm_time;

	if (clock_is_24h_style()) {
		strncpy(time_format, "%R", sizeof(time_format));
	} else {
		strncpy(time_format, "%I:%M", sizeof(time_format));
	}

	batteries[0] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_0);
	batteries[1] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_1);
	batteries[2] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_2);
	batteries[3] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_3);
	batteries[4] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_4);
	batteries[5] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_5);
	
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_set_background_color(window, GColorBlack);

	Layer *window_layer = window_get_root_layer(window);

	date_layer = text_layer_create(GRect(2, 0, 100, 22));
	text_layer_set_text_color(date_layer, GColorWhite);
	text_layer_set_background_color(date_layer, GColorBlack);
	text_layer_set_text_alignment(date_layer, GTextAlignmentLeft);
	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	battery_layer = bitmap_layer_create(GRect(123, 3, 21, 16));
	bitmap_layer_set_alignment(battery_layer, GAlignRight);
	bitmap_layer_set_background_color(battery_layer, GColorBlack);
	layer_add_child(window_layer, bitmap_layer_get_layer(battery_layer));

	time_layer = text_layer_create(GRect(0, 122, 144, 44));
	text_layer_set_text_color(time_layer, GColorWhite);
	text_layer_set_background_color(time_layer, GColorBlack);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	(void)time(&current_time);
	tm_time = localtime(&current_time);
	handle_minute_tick(tm_time);

	beam_init(window);
	heartbeat_init(window);

	const bool animated = true;
	window_stack_push(window, animated);

	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void) {
	tick_timer_service_unsubscribe();
	beam_deinit();
	text_layer_destroy(time_layer);
	text_layer_destroy(date_layer);
	window_destroy(window);

	for (int i = 0; i < 6; i++) {
		gbitmap_destroy(batteries[i]);
	}
}

static void window_load(Window *window) {
}

static void window_unload(Window *window) {
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed)
{
	switch (tick_time->tm_sec) {
		case 0:
			handle_minute_tick(tick_time);
			beam_clear();
			heartbeat_reset_animation();
			beam_shift();
			break;
		case 10:
			beam_reset_animation(beam_bands[0], NULL, NULL);
			break;
		case 20:
			beam_reset_animation(beam_bands[1], beam_bands[2], NULL);
			break;
		case 30:
			beam_reset_animation(beam_bands[3], beam_bands[4], beam_bands[5]);
			break;
		case 40:
			beam_reset_animation(NULL, beam_bands[6], beam_bands[7]);
			break;
		case 50:
			beam_reset_animation(NULL, NULL, beam_bands[8]);
			break;
	}
}

static void handle_minute_tick(struct tm *tick_time)
{
	set_battery_bitmap();

	strftime(date_text, sizeof(date_text), "%B %e", tick_time);
	text_layer_set_text(date_layer, date_text);

	strftime(time_text, sizeof(time_text), time_format, tick_time);
	if (!clock_is_24h_style() && *time_text == '0') {
		text_layer_set_text(time_layer, time_text + 1);
	} else {
		text_layer_set_text(time_layer, time_text);
	}
}

static void set_battery_bitmap()
{
	BatteryChargeState battery = battery_state_service_peek();
	int percent = battery.charge_percent;
	int index;

	if (percent >= 95) {
		index = 5;
	} else if (percent >= 80) {
		index = 4;
	} else if (percent >= 55) {
		index = 3;
	} else if (percent >= 30) {
		index = 2;
	} else if (percent > 5) {
		index = 1;
	} else {
		index = 0;
	}

	bitmap_layer_set_bitmap(battery_layer, batteries[index]);
}
