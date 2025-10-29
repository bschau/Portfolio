#include "include.h"

static int find_first_uncompleted_day(void);
static void click_config_provider(void *context);
static void window_load(Window *window);
static int build_day_layer(Layer *window_layer, GRect bounds, int y_pos);
static int build_day_number_layer(Layer *window_layer, GRect bounds, int y_pos);
static int build_information_layer(Layer *window_layer, GRect bounds, int y_pos);
static int build_completed_layer(Layer *window_layer, GRect bounds, int y_pos);
static void window_appear(Window *window);
static void window_unload(Window *window);
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void down_click_handler(ClickRecognizerRef recognizer, void *context);
static void set_day_string(void);
static void set_information_string(void);
static void set_completed_string(void);
static void redraw_window(void);

static Window *window;
static TextLayer *day_layer;
static TextLayer *day_number_layer;
static TextLayer *information_layer;
static TextLayer *completed_layer;
static char day_number_buffer[8];
static char information_buffer[16];

void view_main_init(void)
{
	day = find_first_uncompleted_day();
	window = window_new(click_config_provider, window_load, window_unload, window_appear);
}

static int find_first_uncompleted_day()
{
	int index = selected_user * (MAX_DAYS / 8), bit = 1, i_day = 0;

	for (i_day = 0; i_day < MAX_DAYS; i_day++) {
		if (!(user_profiles[selected_user].completed[index] & bit)) {
			return i_day;
		}

		bit <<= 1;
		if (bit > 128) {
			bit = 1;
			index++;
		}
	}

	return 0;
}

static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	int y_pos = 2;

	y_pos += build_day_layer(window_layer, bounds, y_pos);
	y_pos += build_day_number_layer(window_layer, bounds, y_pos);
	y_pos += build_information_layer(window_layer, bounds, y_pos);
	y_pos += build_completed_layer(window_layer, bounds, y_pos);
}

static int build_day_layer(Layer *window_layer, GRect bounds, int y_pos)
{
	day_layer = text_layer_create((GRect) { .origin = { 0, y_pos }, .size = { bounds.size.w, 32 } });
	text_layer_set_font(day_layer, small_font);
	text_layer_set_text(day_layer, locale_get(LOCALE_DAY));
	text_layer_set_text_alignment(day_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(day_layer));

	return 32;
}

static int build_day_number_layer(Layer *window_layer, GRect bounds, int y_pos)
{
	day_number_layer = text_layer_create((GRect) { .origin = { 0, y_pos }, .size = { bounds.size.w, 42 } });
	text_layer_set_font(day_number_layer, large_font);
	text_layer_set_text_alignment(day_number_layer, GTextAlignmentCenter);
	set_day_string();
	layer_add_child(window_layer, text_layer_get_layer(day_number_layer));

	return 42 + 6;
}

static int build_information_layer(Layer *window_layer, GRect bounds, int y_pos)
{
	information_layer = text_layer_create((GRect) { .origin = { 0, y_pos }, .size = { bounds.size.w, 28 } });
	text_layer_set_font(information_layer, small_font);
	text_layer_set_text_alignment(information_layer, GTextAlignmentCenter);
	set_information_string();
	layer_add_child(window_layer, text_layer_get_layer(information_layer));

	return 28 + 6;
}

static int build_completed_layer(Layer *window_layer, GRect bounds, int y_pos)
{
	completed_layer = text_layer_create((GRect) { .origin = { 0, y_pos }, .size = { bounds.size.w, 32 } });
	text_layer_set_font(completed_layer, small_font);
	text_layer_set_text_alignment(completed_layer, GTextAlignmentCenter);
	set_completed_string();
	layer_add_child(window_layer, text_layer_get_layer(completed_layer));

	return 32 + 2;
}

static void window_appear(Window *window)
{
	text_layer_set_text(day_layer, locale_get(LOCALE_DAY));
	redraw_window();
}

static void window_unload(Window *window)
{
	text_layer_destroy(day_layer);
	text_layer_destroy(day_number_layer);
	text_layer_destroy(information_layer);
	text_layer_destroy(completed_layer);
	window_destroy(window);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
	view_main_menu_init();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (day < MAX_DAYS - 1) {
		day++;
	}

	program_select_day(day);
	redraw_window();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (day > 0) {
		day--;
	}

	program_select_day(day);
	redraw_window();
}

static void set_day_string(void)
{
	snprintf(day_number_buffer, sizeof(day_number_buffer), "%i", day + 1);
	text_layer_set_text(day_number_layer, day_number_buffer);
}

static void set_information_string(void)
{
	short total_time, run_time;

	program_get_times(&total_time, &run_time);
	snprintf(information_buffer, sizeof(information_buffer), "%i:%02i / %i:%02i",
		total_time / 60,
		total_time % 60,
		run_time / 60,
		run_time % 60);
	text_layer_set_text(information_layer, information_buffer);
}

static void set_completed_string(void)
{
	int index, bit;
	get_day(&index, &bit);

	if (user_profiles[selected_user].completed[index] & bit) {
		text_layer_set_text(completed_layer, locale_get(LOCALE_COMPLETED));
	} else {
		text_layer_set_text(completed_layer, "");
	}
}

static void redraw_window(void)
{
	set_day_string();
	set_information_string();
	set_completed_string();

	layer_mark_dirty(text_layer_get_layer(day_layer));
	layer_mark_dirty(text_layer_get_layer(day_number_layer));
	layer_mark_dirty(text_layer_get_layer(information_layer));
	layer_mark_dirty(text_layer_get_layer(completed_layer));
}

