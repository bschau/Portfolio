#include "include.h"

static void window_load(Window *window);
static void build_menu(void);
static void window_unload(Window *window);
static void eggtimer_callback(int menu_index, void *context);
static void stopwatch_callback(int menu_index, void *context);

static Window *window;
static SimpleMenuLayer *menu_layer;
static SimpleMenuSection menu_sections[1];
static SimpleMenuItem menu_items[3];

void view_main_init(void)
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
		.title = "Egg Timer",
		.callback = eggtimer_callback,
		.icon = eggtimer_icon,
	};

	menu_items[index++] = (SimpleMenuItem) {
		.title = "Stopwatch",
		.callback = stopwatch_callback,
		.icon = stopwatch_icon,
	};

	menu_items[index++] = (SimpleMenuItem) {
		.title = "wwTimer",
		.subtitle = "V2 (c) schau.dk",
	};

	menu_sections[0] = (SimpleMenuSection) {
		.num_items = index,
		.items = menu_items,
	};
}

static void window_unload(Window *window)
{
	simple_menu_layer_destroy(menu_layer);
	window_destroy(window);
}

static void eggtimer_callback(int menu_index, void *context)
{
	view_eggtimer_main_init();
}

static void stopwatch_callback(int menu_index, void *context)
{
	view_stopwatch_main_init();
}
