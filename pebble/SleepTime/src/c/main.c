#include <pebble.h>
#define SCREENSHOTTING (0)

typedef enum {
	AppKeyNightStart2 = 2,
	AppKeyNightStop2 = 3
} AppKey;

static void load_environment(void);
static void save_environment(void);
static void init(void);
static void deinit(void);
static void window_load(Window *window);
static void window_unload(Window *window);
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed);
static void set_day_night_display(struct tm *tick_time);
static void set_day_display(void);
static void set_night_display(void);
static void set_battery_indicator(void);
static void inbox_received_handler(DictionaryIterator *iter, void *context);
static int get_time(char *src);
 
static GBitmap *day_png, *night_png;
static Window *window;
static BitmapLayer *background_layer;
static TextLayer *time_layer, *battery_layer;
static char time_text[] = "00:00";
static char time_format[6];
static char battery_buffer[2];
static int night_ends = (6 * 60);
static int night_begins = (19 * 60) + 30;

int main(void)
{
	load_environment();
	init();
	app_event_loop();
	deinit();
	save_environment();
}

static void load_environment()
{
	int value = persist_read_int(AppKeyNightStart2);
	night_begins = value == 0 ? (19 * 60) + 30 : value;

	value = persist_read_int(AppKeyNightStop2);
	night_ends = value == 0 ? (6 * 60) : value;
}

static void save_environment()
{
	persist_write_int(AppKeyNightStart2, night_begins);
	persist_write_int(AppKeyNightStop2, night_ends);
}

static void init(void)
{
	int x, y;

	setlocale(LC_ALL, "da");
	if (clock_is_24h_style()) {
		strncpy(time_format, "%R", sizeof(time_format));
	} else {
		strncpy(time_format, "%I:%M", sizeof(time_format));
	}

	day_png = gbitmap_create_with_resource(RESOURCE_ID_DAY);
	night_png = gbitmap_create_with_resource(RESOURCE_ID_NIGHT);
	
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
        });

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	x = (bounds.size.w - 144) / 2;
	y = (bounds.size.h - 168) / 2;

#if PBL_ROUND
	y += 2;
#endif
	background_layer = bitmap_layer_create(GRect(x, y + 1, 144, 128));
	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

#if PBL_ROUND
	time_layer = text_layer_create(GRect(x, y + 118, 144, 44));
#else
	time_layer = text_layer_create(GRect(x, y + 122, 144, 44));
#endif
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

#if PBL_ROUND
	battery_layer = text_layer_create(GRect(bounds.size.w - 24, (bounds.size.h - 20) / 2, 20, 20));
#else
	battery_layer = text_layer_create(GRect(x + 120, y, 20, 20));
#endif
	text_layer_set_background_color(battery_layer, GColorClear);
	battery_buffer[0] = '0';
	battery_buffer[1] = 0;
	text_layer_set_text(battery_layer, battery_buffer);
	text_layer_set_text_alignment(battery_layer, GTextAlignmentRight);
	text_layer_set_font(battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(battery_layer));

	window_stack_push(window, true);
}

static void deinit(void)
{
	text_layer_destroy(time_layer);
	bitmap_layer_destroy(background_layer);
	window_destroy(window);
	gbitmap_destroy(day_png);
	gbitmap_destroy(night_png);
}

static void window_load(Window *window)
{
	time_t current_time;
	struct tm *tm_time;

	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);

	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(200, 0);

	(void)time(&current_time);
	tm_time = localtime(&current_time);
	handle_minute_tick(tm_time, MINUTE_UNIT);
}

static void window_unload(Window *window)
{
	app_message_deregister_callbacks();
	tick_timer_service_unsubscribe();
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{
	set_day_night_display(tick_time);

	strftime(time_text, sizeof(time_text), time_format, tick_time);
	if (!clock_is_24h_style() && *time_text == '0') {
		text_layer_set_text(time_layer, time_text + 1);
	} else {
		text_layer_set_text(time_layer, time_text);
	}
	
	set_battery_indicator();
}

static void set_day_night_display(struct tm *tick_time)
{
#if SCREENSHOTTING == 0
	struct tm* converted_time = tick_time;
	int now = (converted_time->tm_hour * 60) + converted_time->tm_min;
	
	if (now < night_ends || now >= night_begins) {
		set_night_display();
		return;
	}

	set_day_display();
#elif SCREENSHOTTING == 1
	set_night_display();
#else
	set_day_display();
#endif
}

static void set_day_display(void)
{
	window_set_background_color(window, GColorWhite);
	text_layer_set_text_color(time_layer, GColorBlack);
	text_layer_set_text_color(battery_layer, GColorBlack);
	bitmap_layer_set_bitmap(background_layer, day_png);
}

static void set_night_display(void)
{
	window_set_background_color(window, GColorBlack);
	text_layer_set_text_color(time_layer, GColorWhite);
	text_layer_set_text_color(battery_layer, GColorWhite);
	bitmap_layer_set_bitmap(background_layer, night_png);
}

static void set_battery_indicator(void)
{
	BatteryChargeState battery = battery_state_service_peek();
	int index = battery.charge_percent / 10;

	if (index < 1) {
		battery_buffer[0] = '0';
	} else if (index > 9) {
		battery_buffer[0] = '9';
	} else {
		battery_buffer[0] = index + '0';
	}
}

static void inbox_received_handler(DictionaryIterator *iterator, void *context) {
	Tuple *tuple = dict_find(iterator, AppKeyNightStart2);
	if (tuple) {
		night_begins = get_time(tuple->value->cstring);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Night begins: %i", night_begins);
	}
	
	tuple = dict_find(iterator, AppKeyNightStop2);
	if (tuple) {
		night_ends = get_time(tuple->value->cstring);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Night ends: %i", night_ends);
	}

	vibes_double_pulse();
}

static int get_time(char *src)
{
	int hour, minute;
	char *p;

	p = strchr(src, ':');
	if (p == NULL) {
		return (6 * 60);
	}
	*p = 0;

	hour = atoi(src);
	minute = atoi(p + 1);

	return (hour * 60) + minute;
}
