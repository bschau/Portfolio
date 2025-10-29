#include <pebble.h>

static void init(void);
static void deinit(void);
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed);
static void set_battery_bitmap();

static GBitmap *background_png;
static GBitmap *batteries[10] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static Window *window;
static BitmapLayer *background_layer, *battery_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;
static char time_text[] = "00:00";
static char date_text[] = "Xxxxxxxxx 00";
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
		strncpy(time_format, "%R", sizeof(time_format));
	} else {
		strncpy(time_format, "%I:%M", sizeof(time_format));
	}

	background_png = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
	batteries[0] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_0);
	batteries[1] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_1);
	batteries[2] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_2);
	batteries[3] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_3);
	batteries[4] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_4);
	batteries[5] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_5);
	batteries[6] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_6);
	batteries[7] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_7);
	batteries[8] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_8);
	batteries[9] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_9);
	
	window = window_create();
	window_set_background_color(window, GColorWhite);
	Layer *window_layer = window_get_root_layer(window);

	background_layer = bitmap_layer_create(GRect(0, 28, 144, 100));
	bitmap_layer_set_bitmap(background_layer, background_png);
	bitmap_layer_set_alignment(background_layer, GAlignCenter|GAlignTop);
	bitmap_layer_set_background_color(background_layer, GColorClear);
	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

	date_layer = text_layer_create(GRect(2, 0, 100, 22));
	text_layer_set_text_color(date_layer, GColorBlack);
	text_layer_set_background_color(date_layer, GColorClear);
	text_layer_set_text_alignment(date_layer, GTextAlignmentLeft);
	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	battery_layer = bitmap_layer_create(GRect(118, 6, 24, 14));
	bitmap_layer_set_alignment(battery_layer, GAlignRight);
	bitmap_layer_set_background_color(battery_layer, GColorClear);
	layer_add_child(window_layer, bitmap_layer_get_layer(battery_layer));

	time_layer = text_layer_create(GRect(0, 122, 144, 44));
	text_layer_set_text_color(time_layer, GColorBlack);
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	set_battery_bitmap();

	window_stack_push(window, true);
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void)
{
	int i;

	tick_timer_service_unsubscribe();
	text_layer_destroy(time_layer);
	text_layer_destroy(date_layer);
	bitmap_layer_destroy(background_layer);
	window_destroy(window);

	for (i = 0; i < 10; i++) {
		gbitmap_destroy(batteries[i]);
	}

	gbitmap_destroy(background_png);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
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
	
	if (percent > 99) {
		percent = 99;
	}

	bitmap_layer_set_bitmap(battery_layer, batteries[percent / 10]);
}
