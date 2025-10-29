#define MAIN
#include "lights.h"

static void init(void);
static void deinit(void);
static void window_load(Window *window);
static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed);
static void handle_minute_tick(struct tm *tick_time);
static void set_heart_bitmap();

static char time_text[] = "00:00";
static char date_text[] = "Xxxxxxxxx 00";
static BitmapLayer *heart_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;
static char time_format[6];
static GBitmap *hearts[11];
static Window *window;
static LightsLayer *lights_layer;

int main(void)
{
	init();
	app_event_loop();
	deinit();
}

static void init(void)
{
	time_t current_time;
	struct tm *tm_time;

	if (clock_is_24h_style()) {
		strncpy(time_format, "%R", sizeof(time_format));
	} else {
		strncpy(time_format, "%I:%M", sizeof(time_format));
	}

	hearts[0] = gbitmap_create_with_resource(RESOURCE_ID_HEART0);
	hearts[1] = gbitmap_create_with_resource(RESOURCE_ID_HEART1);
	hearts[2] = gbitmap_create_with_resource(RESOURCE_ID_HEART2);
	hearts[3] = gbitmap_create_with_resource(RESOURCE_ID_HEART3);
	hearts[4] = gbitmap_create_with_resource(RESOURCE_ID_HEART4);
	hearts[5] = gbitmap_create_with_resource(RESOURCE_ID_HEART5);
	hearts[6] = gbitmap_create_with_resource(RESOURCE_ID_HEART6);
	hearts[7] = gbitmap_create_with_resource(RESOURCE_ID_HEART7);
	hearts[8] = gbitmap_create_with_resource(RESOURCE_ID_HEART8);
	hearts[9] = gbitmap_create_with_resource(RESOURCE_ID_HEART9);
	hearts[10] = gbitmap_create_with_resource(RESOURCE_ID_HEARTF);

	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
	});
	window_set_background_color(window, GColorBlack);

	Layer *window_layer = window_get_root_layer(window);

	lights_layer = lights_layer_create(GPoint(5, 8));
	layer_add_child(window_layer, lights_layer_get_layer(lights_layer));

	time_layer = text_layer_create(GRect(12, 18, 119, 40));
	text_layer_set_text_color(time_layer, GColorWhite);
	text_layer_set_background_color(time_layer, GColorBlack);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	date_layer = text_layer_create(GRect(12, 63, 119, 22));
	text_layer_set_text_color(date_layer, GColorWhite);
	text_layer_set_background_color(date_layer, GColorBlack);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	heart_layer = bitmap_layer_create(GRect(48, 96, 47, 43));
	bitmap_layer_set_background_color(heart_layer, GColorBlack);
	layer_add_child(window_layer, bitmap_layer_get_layer(heart_layer));

	(void)time(&current_time);
	tm_time = localtime(&current_time);
	handle_minute_tick(tm_time);

	lights_layer_set_active(lights_layer, tm_time->tm_sec + 1);

	const bool animated = true;
	window_stack_push(window, animated);

	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void)
{
	tick_timer_service_unsubscribe();

	text_layer_destroy(time_layer);
	text_layer_destroy(date_layer);
	bitmap_layer_destroy(heart_layer);

	lights_layer_destroy(lights_layer);
	window_destroy(window);

	for (int i = 0; i < 11; i++) {
		gbitmap_destroy(hearts[i]);
	}
}

static void window_load(Window *window)
{
	layer_mark_dirty(lights_layer_get_layer(lights_layer));
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed)
{
	int secs = tick_time->tm_sec;
	if (secs > 59) {
		secs = 59;
	} else if (secs == 0) {
		handle_minute_tick(tick_time);
		lights_layer_reset_bulbs(lights_layer);
	}

	lights_layer_set_active(lights_layer, secs + 1);
}

static void handle_minute_tick(struct tm *tick_time)
{
	strftime(time_text, sizeof(time_text), time_format, tick_time);
	if (!clock_is_24h_style() && *time_text == '0') {
		text_layer_set_text(time_layer, time_text + 1);
	} else {
		text_layer_set_text(time_layer, time_text);
	}

	strftime(date_text, sizeof(date_text), "%B %e", tick_time);
	text_layer_set_text(date_layer, date_text);

	set_heart_bitmap();
}

static void set_heart_bitmap()
{
	BatteryChargeState battery = battery_state_service_peek();
	int percent = battery.charge_percent;
	int index = percent > 96 ? 10 : percent / 10;
	bitmap_layer_set_bitmap(heart_layer, hearts[index]);
}
