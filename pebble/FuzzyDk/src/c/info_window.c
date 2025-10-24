#include "include.h"

static void SetBatteryBitmap();

static GBitmap *batteries[10];
static BitmapLayer *battery_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;
static char time_text[] = "00:00";
static char date_text[] = "Xxxxxxxxx 00";
static char time_format[6];

void info_window_init(void)
{
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
	batteries[6] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_6);
	batteries[7] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_7);
	batteries[8] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_8);
	batteries[9] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_9);

	info_window = window_create();
	window_set_background_color(info_window, BACKGROUND_COLOR);
	Layer *window_layer = window_get_root_layer(info_window);

	date_layer = text_layer_create(GRect(0, 22, 144, 22));
	text_layer_set_text_color(date_layer, TEXT_COLOR);
	text_layer_set_background_color(date_layer, GColorClear);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	time_layer = text_layer_create(GRect(0, 54, 144, 44));
	text_layer_set_text_color(time_layer, TEXT_COLOR);
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	battery_layer = bitmap_layer_create(GRect(60, 126, 24, 14));
	bitmap_layer_set_alignment(battery_layer, GAlignRight);
	bitmap_layer_set_background_color(battery_layer, GColorClear);
	layer_add_child(window_layer, bitmap_layer_get_layer(battery_layer));

	SetBatteryBitmap();
}

void info_window_deinit(void)
{
	text_layer_destroy(time_layer);
	text_layer_destroy(date_layer);
	bitmap_layer_destroy(battery_layer);
	for (int i = 0; i < 10; i++) {
		gbitmap_destroy(batteries[i]);
	}

	window_destroy(info_window);
}

void info_window_update(struct tm *tick_time)
{
	strftime(date_text, sizeof(date_text), "%B %e", tick_time);
	text_layer_set_text(date_layer, date_text);

	strftime(time_text, sizeof(time_text), time_format, tick_time);
	if (!clock_is_24h_style() && *time_text == '0') {
		text_layer_set_text(time_layer, time_text + 1);
	} else {
		text_layer_set_text(time_layer, time_text);
	}
}

static void SetBatteryBitmap()
{
	BatteryChargeState battery = battery_state_service_peek();
	int index = battery.charge_percent / 10;

	if (index < 1) {
		index = 0;
	} else if (index > 9) {
		index = 9;
	}

	bitmap_layer_set_bitmap(battery_layer, batteries[index]);
}
