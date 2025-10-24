#include <pebble.h>
#define DESIGNS 8
#define DESIGN_WONB_INDEX (0)
#define DESIGN_BONW_INDEX (4)
#define SCREENSHOTTING (0)
#define SCREENSHOT_DESIGN (1)
#define STRESSING (0)

typedef enum {
	AppKeyDesign = 0,
	AppKeyDesign2 = 1,
	AppKeyFiveSec = 2,
	AppKeyWatchType = 3
} AppKey;

static void load_environment(void);
static void save_environment(void);
static void init(void);
static void handle_initial_time(void);
static void set_dots_list(void);
static void deinit(void);
static void window_load(Window *window);
static void window_unload(Window *window);
static void handle_tick(struct tm *tick_time, TimeUnits units_changed);
static void update_dots(int second_index);
static void dots_update_proc(Layer *layer, GContext *ctx);
static void inbox_received_handler(DictionaryIterator *iter, void *context);
static void set_design();
static void select_color_scheme();
static void accel_tap_handler(AccelAxisType axis, int32_t direction);
static void hide_extra_info(void *data);
static void set_battery_status(void);
static void watch_type_two_callback(void *data);

static char time_text[] = "00:00-";
static char date_text[] = "00-";
static char *battery_lines[6] = { "-----", "*", "**", "***", "****", "*****" };
static uint8_t design_backcolor[DESIGNS] = { GColorBlackARGB8, GColorBlackARGB8,
			GColorBlackARGB8, GColorBlackARGB8, GColorWhiteARGB8,
			GColorWhiteARGB8, GColorWhiteARGB8, GColorWhiteARGB8
};
static uint8_t design_forecolor[DESIGNS] = { GColorWhiteARGB8,GColorYellowARGB8,
			GColorMagentaARGB8, GColorCyanARGB8, GColorBlackARGB8,
			GColorDukeBlueARGB8, GColorDarkCandyAppleRedARGB8, GColorIslamicGreenARGB8
};
static Window *window;
static TextLayer *time_layer, *date_layer, *battery_layer;
static Layer *dots_layer;
static char time_format[6];
static GFont lcd_font, date_font;
static int second_index, clear_next_frame;
static int design, five_sec, watch_type;
static int watch_type_data;
static AppTimer *watch_type_timer;
static GPoint *dots;
static int dots_sizes[60];
static int dots_onoff[60];
static GColor back_color, fore_color;
static AppTimer *app_timer;
static time_t tap_time;

int main(void)
{
	watch_type_data = 1;
	load_environment();
	init();
	app_event_loop();
	deinit();
	save_environment();
}

static void load_environment()
{
	design = persist_read_int(AppKeyDesign2);
	five_sec = persist_read_int(AppKeyFiveSec);
	watch_type = persist_read_int(AppKeyWatchType);
#if SCREENSHOTTING == 1
	design = SCREENSHOT_DESIGN;
#endif
#if STRESSING == 1
	design = 0;
	five_sec = 2;
	watch_type = 2;
#endif
}

static void save_environment()
{
	persist_write_int(AppKeyDesign2, design);
	persist_write_int(AppKeyFiveSec, five_sec);
	persist_write_int(AppKeyWatchType, watch_type);
}

static void init(void)
{
	lcd_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCD_50));
	date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCD_18));
	if (clock_is_24h_style()) {
		strncpy(time_format, "%R", sizeof(time_format));
	} else {
		strncpy(time_format, "%I:%M", sizeof(time_format));
	}

	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	Layer *window_layer = window_get_root_layer(window);
	window_set_background_color(window, GColorBlack);
	GRect window_bounds = layer_get_bounds(window_layer);

	dots = (GPoint *)malloc(sizeof(GPoint) * 60);
	set_dots_list();

	dots_layer = layer_create(window_bounds);
	layer_set_update_proc(dots_layer, dots_update_proc);
	layer_add_child(window_layer, dots_layer);

	int w = window_bounds.size.w;
	int h = window_bounds.size.h;
	
	int x = (w - 128) / 2;
	int y = ((h - 60) / 2) - 4;
	time_layer = text_layer_create(GRect(x, y, 128, 60));
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, lcd_font);
	text_layer_set_text_color(time_layer, GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	x = (w - 40) / 2;
	y = (h / 2) + 26;
	date_layer = text_layer_create(GRect(x, y, 40, 20));
	text_layer_set_background_color(date_layer, GColorClear);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
	text_layer_set_font(date_layer, date_font);
	text_layer_set_text_color(date_layer, GColorWhite);
	text_layer_set_text(date_layer, date_text);
	layer_set_hidden(text_layer_get_layer(date_layer), true);
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	x = (w - 100) / 2;
	y = (h / 2) - 44;
	battery_layer = text_layer_create(GRect(x, y, 100, 20));
	text_layer_set_background_color(battery_layer, GColorClear);
	text_layer_set_text_alignment(battery_layer, GTextAlignmentCenter);
	text_layer_set_font(battery_layer, date_font);
	text_layer_set_text_color(battery_layer, GColorWhite);
	layer_set_hidden(text_layer_get_layer(battery_layer), true);
	layer_add_child(window_layer, text_layer_get_layer(battery_layer));

	window_stack_push(window, true);
}

static void handle_initial_time(void)
{
	time_t current_time;
	struct tm *tm_time;

	memset(dots_onoff, 0, sizeof(int) * 60);

	(void)time(&current_time);
	tm_time = localtime(&current_time);
	handle_tick(tm_time, SECOND_UNIT | MINUTE_UNIT | DAY_UNIT);

	// For watch_type = 1
	int index = tm_time->tm_sec > 59 ? 59 : tm_time->tm_sec;
	index = index < 0 ? 0 : index;
	memset(dots_onoff, 1, (index + 1) * sizeof(int));
}

static void set_dots_list(void)
{
	Layer *window_layer = window_get_root_layer(window);
	GRect window_bounds = layer_get_bounds(window_layer);
	GPoint center = grect_center_point(&window_bounds);

	five_sec = five_sec >= 0 || five_sec <= 2 ? five_sec : 0;

#if PBL_ROUND
	const int radius = 78;
#elif PBL_PLATFORM_EMERY
	const int radius = 86;
#else
	const int radius = 69;
#endif
	for (int i = 0; i < 60; i++) {
		dots[i] = (GPoint) {
			.x = ((int16_t)(sin_lookup(TRIG_MAX_ANGLE * i / 60) * (int32_t)radius / TRIG_MAX_RATIO) + center.x) - 1,
			.y = ((int16_t)(-cos_lookup(TRIG_MAX_ANGLE * i / 60) * (int32_t)radius / TRIG_MAX_RATIO) + center.y) - 1
		};
		dots_sizes[i] = i % 5 == 0 ? (2 + five_sec) : 2;
	}
}

static void deinit(void)
{
	text_layer_destroy(date_layer);
	text_layer_destroy(time_layer);
	layer_destroy(dots_layer);
	window_destroy(window);
	fonts_unload_custom_font(lcd_font);
	fonts_unload_custom_font(date_font);
	if (dots != NULL) {
		free(dots);
	}
}

static void window_load(Window *window)
{
	set_design();
	handle_initial_time();
	layer_mark_dirty(text_layer_get_layer(time_layer));

	tick_timer_service_subscribe(SECOND_UNIT|MINUTE_UNIT|DAY_UNIT, handle_tick);
	accel_tap_service_subscribe(&accel_tap_handler);

	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(100, 0);
}

static void window_unload(Window *window)
{
	if (app_timer != NULL) {
		app_timer_cancel(app_timer);
		app_timer = NULL;
	}

	if (watch_type_timer != NULL) {
		app_timer_cancel(watch_type_timer);
		watch_type_timer = NULL;
	}

	app_message_deregister_callbacks();
	accel_tap_service_unsubscribe();
	tick_timer_service_unsubscribe();
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
#if SCREENSHOTTING == 1
	tick_time->tm_hour = 20;
	tick_time->tm_min = 30;
	tick_time->tm_sec = 45;
#endif

	if ((units_changed & DAY_UNIT) == DAY_UNIT) {
		if (tick_time->tm_mday < 10) {
			date_text[0] = tick_time->tm_mday + '0';
			date_text[1] = 0;
		} else {
			date_text[0] = (tick_time->tm_mday / 10) + '0';
			date_text[1] = (tick_time->tm_mday % 10) + '0';
			date_text[2] = 0;
		}
		layer_mark_dirty(text_layer_get_layer(date_layer));
	}

	if ((units_changed & MINUTE_UNIT) == MINUTE_UNIT) {
		strftime(time_text, sizeof(time_text), time_format, tick_time);
		if (!clock_is_24h_style() && *time_text == '0') {
			text_layer_set_text(time_layer, time_text + 1);
		} else {
			text_layer_set_text(time_layer, time_text);
		}
		layer_mark_dirty(text_layer_get_layer(time_layer));
		set_battery_status();
	}

	second_index = tick_time->tm_sec < 0 ? 0 : tick_time->tm_sec % 60;
	update_dots(second_index);
	layer_mark_dirty(dots_layer);
}

static void update_dots(int second_index)
{
	if (watch_type == 0) {
		memset(dots_onoff, 0, sizeof(int) * 60);
		memset(dots_onoff, 1, sizeof(int) * (second_index + 1));
		return;
	}

	if (watch_type == 1) {
		if (second_index == 0) {
			watch_type_data = (watch_type_data + 1) & 1;
		}
		memset(dots_onoff, watch_type_data, sizeof(int) * (second_index + 1));
		return;
	}

	memset(dots_onoff, 1, sizeof(int) * (second_index + 1));
	if (second_index == 0) {
		watch_type_data = 1;
		watch_type_timer = app_timer_register(50, watch_type_two_callback, NULL);
	}
}

static void dots_update_proc(Layer *layer, GContext *ctx)
{
	int i;

	if (second_index == 0 || clear_next_frame) {
		GRect bounds = layer_get_bounds(layer);
		graphics_context_set_fill_color(ctx, back_color);
		graphics_fill_rect(ctx, bounds, 0, GCornerNone);
		clear_next_frame = 0;
	}

	graphics_context_set_fill_color(ctx, fore_color);

	const int ANTIALIASING = true;
	graphics_context_set_antialiased(ctx, ANTIALIASING);

	for (i = 0; i < 60 ; i++) {
		if (dots_onoff[i]) {
			graphics_fill_circle(ctx, dots[i], dots_sizes[i]);
		}
	}
}

static void inbox_received_handler(DictionaryIterator *iterator, void *context)
{
	int l_design = design, l_five_sec = five_sec, l_watch_type = watch_type;

	Tuple *tuple = dict_find(iterator, AppKeyDesign2);
	if (tuple) {
		design = atoi(tuple->value->cstring);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Design: %i", design);
	}

	tuple = dict_find(iterator, AppKeyFiveSec);
	if (tuple) {
		five_sec = atoi(tuple->value->cstring);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Five Sec: %i", five_sec);
	}

	tuple = dict_find(iterator, AppKeyWatchType);
	if (tuple) {
		watch_type = atoi(tuple->value->cstring);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Watch Type: %i", watch_type);
	}

	if (design == l_design && l_five_sec == five_sec && l_watch_type == watch_type) {
		return;
	}

	watch_type_data = 1;

	set_dots_list();
	set_design();
	handle_initial_time();
	vibes_short_pulse();
}

static void set_design()
{
	clear_next_frame = 1;
	select_color_scheme();
	window_set_background_color(window, back_color);
	text_layer_set_text_color(time_layer, fore_color);
	layer_mark_dirty(text_layer_get_layer(time_layer));
	text_layer_set_text_color(date_layer, fore_color);
	layer_mark_dirty(text_layer_get_layer(date_layer));
	text_layer_set_text_color(battery_layer, fore_color);
	layer_mark_dirty(text_layer_get_layer(battery_layer));
}

static void select_color_scheme()
{
	if (design > (DESIGNS - 1) || design < 0) {
		design = 0;
	}

#ifndef PBL_COLOR
	design = design_backcolor[design] == GColorWhiteARGB8 ? DESIGN_BONW_INDEX : DESIGN_WONB_INDEX;
#endif

	fore_color = (GColor){ .argb = design_forecolor[design] };
	back_color = (GColor){ .argb = design_backcolor[design] };
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
		app_timer = NULL;
	}

	layer_set_hidden(text_layer_get_layer(date_layer), false);
	layer_set_hidden(text_layer_get_layer(battery_layer), false);
	app_timer = app_timer_register(3000, hide_extra_info, NULL);
}

static void hide_extra_info(void *data)
{
	app_timer = NULL;
	layer_set_hidden(text_layer_get_layer(date_layer), true);
	layer_set_hidden(text_layer_get_layer(battery_layer), true);
}

static void set_battery_status(void)
{
	BatteryChargeState chargeState = battery_state_service_peek();
	int index;

	if (chargeState.charge_percent < 10) {
		index = 0;
	} else if (chargeState.charge_percent < 30) {
		index = 1;
	} else if (chargeState.charge_percent < 50) {
		index = 2;
	} else if (chargeState.charge_percent < 70) {
		index = 3;
	} else if (chargeState.charge_percent < 90) {
		index = 4;
	} else {
		index = 5;
	}

	text_layer_set_text(battery_layer, battery_lines[index]);
}

static void watch_type_two_callback(void *data)
{
	dots_onoff[watch_type_data] = 0;
	watch_type_data++;
	if (watch_type_data < 60) {
		watch_type_timer = app_timer_register(50, watch_type_two_callback, NULL);
	} else {
		watch_type_timer = NULL;
	}

	layer_mark_dirty(dots_layer);
}
