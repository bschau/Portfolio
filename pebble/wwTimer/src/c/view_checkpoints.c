#include "include.h"

#define LINE_WIDTH 16

static void window_load(Window *window);
static void build_menu(void);
static void window_unload(Window *window);

static Window *window;
static SimpleMenuLayer *menu_layer;
static SimpleMenuSection menu_sections[1];
static SimpleMenuItem menu_items[MAX_CHECKPOINTS];
static char menu_items_buffer[MAX_CHECKPOINTS * LINE_WIDTH];

void view_checkpoints_init(void)
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
	char *buffer = menu_items_buffer;
	struct tm *tm;
	int index;

	for (index = 0; index < checkpoint_index; index++) {
		tm = gmtime(&checkpoints[index].time);
		snprintf(buffer, LINE_WIDTH, "%d:%02d:%02d.%03i", tm->tm_hour, tm->tm_min, tm->tm_sec, checkpoints[index].millis); 
		menu_items[index] = (SimpleMenuItem) {
			.title = buffer,
		};
		buffer += LINE_WIDTH;
	}

	menu_sections[0] = (SimpleMenuSection) {
		.num_items = checkpoint_index,
		.items = menu_items,
	};
}

static void window_unload(Window *window)
{
	simple_menu_layer_destroy(menu_layer);
	window_destroy(window);
}
