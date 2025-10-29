#include "include.h"

static void window_load(Window *window);
static void build_menu(void);
static char *get_completed_string(void);
static void window_unload(Window *window);
static void change_completed_callback(int menu_index, void *context);
static void change_language_callback(int menu_index, void *context);
static void change_multiuser_callback(int menu_index, void *context);
static void run_callback(int menu_index, void *context);

static Window *window;
static SimpleMenuLayer *menu_layer;
static SimpleMenuSection menu_sections[1];
static SimpleMenuItem menu_items[5];

void view_main_menu_init(void)
{
	window = window_new(NULL, window_load, window_unload, NULL);
}

static void window_load(Window *window)
{
	build_menu();

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	menu_layer = simple_menu_layer_create(bounds, window, menu_sections, 1, NULL);
	layer_add_child(window_layer, simple_menu_layer_get_layer(menu_layer));
}

static void build_menu(void)
{
	int index = 0;

	menu_items[index++] = (SimpleMenuItem) {
		.title = locale_get(LOCALE_RUN),
		.callback = run_callback,
	};

	menu_items[index++] = (SimpleMenuItem) {
		.title = locale_get(LOCALE_COMPLETED),
		.subtitle = get_completed_string(),
		.callback = change_completed_callback,
	};

	menu_items[index++] = (SimpleMenuItem) {
		.title = locale_get(LOCALE_LANGUAGE),
		.subtitle = locale_get(LOCALE_ENGLISH + user_profiles[selected_user].language),
		.callback = change_language_callback,
	};

	menu_items[index++] = (SimpleMenuItem) {
		.title = locale_get(LOCALE_MULTIUSER),
		.subtitle = locale_get(LOCALE_NO + multi_user),
		.callback = change_multiuser_callback,
	};

	menu_items[index++] = (SimpleMenuItem) {
		.title = "First5km",
		.subtitle = "V2 (c) schau.dk",
	};

	menu_sections[0] = (SimpleMenuSection) {
		.num_items = index,
		.items = menu_items,
	};
}

static char *get_completed_string(void)
{
	int index, bit;
	get_day(&index, &bit);

	if (user_profiles[selected_user].completed[index] & bit) {
		return locale_get(LOCALE_YES);
	}

	return locale_get(LOCALE_NO);
}

static void window_unload(Window *window)
{
	simple_menu_layer_destroy(menu_layer);
	window_destroy(window);
}

static void change_completed_callback(int menu_index, void *context)
{
	int index, bit;
	get_day(&index, &bit);

	if (user_profiles[selected_user].completed[index] & bit) {
		user_profiles[selected_user].completed[index] &= (255 - bit);
	} else {
		user_profiles[selected_user].completed[index] |= bit;
	}
	build_menu();
	layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
}

static void change_language_callback(int menu_index, void *context)
{
	if (user_profiles[selected_user].language == LOCALE_LANGUAGE_ENGLISH) {
		user_profiles[selected_user].language = LOCALE_LANGUAGE_DANISH;
	} else {
		user_profiles[selected_user].language = LOCALE_LANGUAGE_ENGLISH;
	}

	build_menu();
	layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
}

static void change_multiuser_callback(int menu_index, void *context)
{
	multi_user++;
	multi_user &= 1;
	build_menu();
	layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
}

static void run_callback(int menu_index, void *context)
{
	view_run_init();
}

