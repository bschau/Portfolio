#include "include.h"

static void force_update(void);
static struct tm *get_time(void);
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed);
static void set_text(int index, int hour);
static void animation_update(void *data);
static void accel_tap_handler(AccelAxisType axis, int32_t direction);
static void auto_close_info_window(void *data);

static char *hour_texts[] = { "tolv", "et", "to", "tre", "fire", "fem", "seks", "syv", "otte", "ni", "ti", "elleve", "tolv" };
static uint8_t highlights[] = { GColorOxfordBlueARGB8, GColorBlueARGB8, GColorArmyGreenARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8, GColorLimerickARGB8, GColorOrangeARGB8, GColorChromeYellowARGB8, GColorWindsorTanARGB8, GColorRedARGB8, GColorDarkCandyAppleRedARGB8, GColorBulgarianRoseARGB8, GColorOxfordBlueARGB8 };
struct {
	int hour_adjustment;
	char *texts[4];
} fuzzy_texts[] = {
	{ 0, { "", "", "%s", "" } },
	{ 0, { "lidt", "over", "%s", "" } },
	{ 0, { "", "fem", "over", "%s" } },
	{ 0, { "ti", "over", "%s", "" } },
	{ 0, { "", "kvart", "over", "%s" } },
	{ 1, { "ti", "i", "halv", "%s" } },
	{ 1, { "fem", "i", "halv", "%s" } },
	{ 1, { "", "halv", "%s", "" } },
	{ 1, { "fem", "over", "halv", "%s" } },
	{ 1, { "ti", "over", "halv", "%s" } },
	{ 1, { "", "kvart", "i", "%s" } },
	{ 1, { "ti", "i", "%s", "" } },
	{ 1, { "", "fem", "i", "%s" } },
	{ 1, { "lidt", "i", "%s", "" } }
};

#define LINES_WIDTH (7)
static char *lines[4] = { "1111111", "2222222", "3333333", "4444444" };
static char *dst_lines[4] = { "5555555", "6666666", "7777777", "8888888" };
static TextLayer *text_layers[4] = { NULL, NULL, NULL, NULL };
static AppTimer *app_timer;
static int last_index;
static time_t tap_time;

void main_window_init(void)
{
	main_window = window_create();
	window_stack_push(main_window, true);
	window_set_background_color(main_window, BACKGROUND_COLOR);
	Layer *window_layer = window_get_root_layer(main_window);
 
	int y_pos = 8;
	for (int index = 0; index < 4; index++) {
		TextLayer *text_layer = text_layer_create(GRect(0, y_pos, 144, 36));
		text_layer_set_text_color(text_layer, TEXT_COLOR);
		text_layer_set_background_color(text_layer, GColorClear);
		text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
		
		memset(lines[index], ' ', LINES_WIDTH);
		text_layer_set_text(text_layer, lines[index]);
		
		layer_add_child(window_layer, text_layer_get_layer(text_layer));
		text_layers[index] = text_layer;
		y_pos += 38;
	}

	force_update();

	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	accel_tap_service_subscribe(&accel_tap_handler);
}

static void force_update(void)
{
	last_index = -1;
	struct tm *local_now = get_time();
	handle_minute_tick(local_now, 0);
}

static struct tm *get_time(void)
{
	time_t now = time(NULL);
	return localtime(&now);
}

void main_window_deinit(void)
{
	accel_tap_service_unsubscribe();
	tick_timer_service_unsubscribe();
	if (app_timer != NULL) {
		app_timer_cancel(app_timer);
	}

	for (int index = 0; index < 4; index++) {
		text_layer_destroy(text_layers[index]);
	}

	window_destroy(main_window);
}

static void handle_minute_tick(struct tm *now, TimeUnits units_changed)
{
	int hour = now->tm_hour;

	if (now->tm_min == 0) {
		set_text(0, hour);
		return;
	}

	if (now->tm_min == 1 || now->tm_min == 2) {
		set_text(1, hour);
	} else if (now->tm_min >= 3 && now->tm_min <= 7) {
		set_text(2, hour);
	} else if (now->tm_min >= 8 && now->tm_min <= 12) {
		set_text(3, hour);
	} else if (now->tm_min >= 13 && now->tm_min <= 17) {
		set_text(4, hour);
	} else if (now->tm_min >= 18 && now->tm_min <= 22) {
		set_text(5, hour);
	} else if (now->tm_min >= 23 && now->tm_min <= 27) {
		set_text(6, hour);
	} else if (now->tm_min >= 28 && now->tm_min <= 32) {
		set_text(7, hour);
	} else if (now->tm_min >= 33 && now->tm_min <= 37) {
		set_text(8, hour);
	} else if (now->tm_min >= 38 && now->tm_min <= 42) {
		set_text(9, hour);
	} else if (now->tm_min >= 43 && now->tm_min <= 47) {
		set_text(10, hour);
	} else if (now->tm_min >= 48 && now->tm_min <= 52) {
		set_text(11, hour);
	} else if (now->tm_min >= 53 && now->tm_min <= 57) {
		set_text(12, hour);
	} else {
		set_text(13, hour);
	}
}

static void set_text(int index, int hour)
{
	if (index == last_index) {
		return;
	}

	last_index = index;
	hour += fuzzy_texts[index].hour_adjustment;
	if (hour > 12) {
		hour -= 12;
	}

	for (int i = 0; i < 4; i++) {
		memset(dst_lines[i], 0, LINES_WIDTH);
		int size = snprintf(dst_lines[i], LINES_WIDTH, fuzzy_texts[index].texts[i], hour_texts[hour]);
		dst_lines[i][size] = 0;

		TextLayer *text_layer = text_layers[i];
		if (fuzzy_texts[index].texts[i][0] == '%') {
			text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
#if PBL_COLOR
			text_layer_set_text_color(text_layer, (GColor)highlights[index]);
#else
			text_layer_set_text_color(text_layer, HIGHLIGHT_COLOR);
#endif
		} else {
			text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
			text_layer_set_text_color(text_layer, TEXT_COLOR);
		}
	}

	app_timer = app_timer_register(100, animation_update, NULL);
}

static void animation_update(void *data)
{
	int bumped_one = 0;

	for (int i = 0; i < 4; i++) {
		char *d = dst_lines[i];
		char *l = lines[i];
		for (int j = 0; j < LINES_WIDTH; j++, d++, l++) {
			if (*l == *d) {
				continue;
			}

			if (*l < *d) {
				++*l;
				if (*l < 'a') {
					*l = 'a';
				}
			} else {
				--*l;
				if (*l < 'a') {
					*l = 0;
				}
			}
			bumped_one = 1;
		}
		layer_mark_dirty(text_layer_get_layer(text_layers[i]));
	}

	if (bumped_one) {
		app_timer = app_timer_register(100, animation_update, NULL);
	} else {
		app_timer = NULL;
	}
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction)
{
	time_t now = time(NULL);

	if (now - tap_time > 3) {
		tap_time = now;
		return;
	}

	if (app_timer != NULL) {
		app_timer_cancel(app_timer);
	}

	info_window_update(get_time());
	window_stack_push(info_window, true);
	app_timer = app_timer_register(3000, auto_close_info_window, NULL);
}

static void auto_close_info_window(void *data)
{
	app_timer = NULL;
	force_update();
	const bool animated = false;
	window_stack_pop_all(animated);
	window_stack_push(main_window, true);
}
