#include <pebble.h>
#include "InfoLayer.h"

#define WIDTH (144)
#define HEIGHT (42)
#define BLUE_W (34)
#define FONT_H (24)

typedef struct {
	GBitmap *batteries[10];
	char date_text[14];
	int battery_index;
	GFont font;
	AppTimer *app_timer;
	time_t tap_time;
} LayerData;

static int battery_ids[10] = { RESOURCE_ID_BAT_0, RESOURCE_ID_BAT_1, RESOURCE_ID_BAT_2, RESOURCE_ID_BAT_3, RESOURCE_ID_BAT_4,
			      RESOURCE_ID_BAT_5, RESOURCE_ID_BAT_6, RESOURCE_ID_BAT_7, RESOURCE_ID_BAT_8, RESOURCE_ID_BAT_9 };

static void update_proc(Layer *layer, GContext *ctx);
static void hide_info(void *data);

static void update_proc(Layer *layer, GContext *ctx)
{
	LayerData *data = (LayerData *)layer_get_data(layer);
	int width = ((InfoLayer *)layer)->width;

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, GRect(0, 0, width - BLUE_W, HEIGHT), 0, 0);

	graphics_context_set_text_color(ctx, GColorBlack);
	graphics_draw_text(ctx, data->date_text, data->font, GRect(1, 2, width - BLUE_W - 2, FONT_H), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

	graphics_context_set_fill_color(ctx, GColorPictonBlue);
	graphics_fill_rect(ctx, GRect(width - BLUE_W, 0, BLUE_W, HEIGHT), 0, 0);

	graphics_draw_bitmap_in_rect(ctx, data->batteries[data->battery_index], GRect(width - BLUE_W + ((BLUE_W - 14) / 2), (HEIGHT - 24) / 2, 14, 24));
}

InfoLayer *info_layer_create(int width, int height)
{
	InfoLayer *layer = malloc(sizeof(InfoLayer));
#if PBL_ROUND
	layer->layer = layer_create_with_data(GRect(0, (height - HEIGHT) / 2, width, HEIGHT), sizeof(LayerData));
#else
	layer->layer = layer_create_with_data(GRect(0, height - HEIGHT, width, HEIGHT), sizeof(LayerData));
#endif
	layer->width = width;
	layer_set_hidden(layer->layer, true);
	layer_set_update_proc(layer->layer, (LayerUpdateProc)update_proc);
  
	LayerData *data = (LayerData *)layer_get_data(layer->layer);
	data->date_text[0] = 0;
	data->battery_index = 0;
	for (int i = 0; i < 10; i++) {
		data->batteries[i] = gbitmap_create_with_resource(battery_ids[i]);
	}

	data->font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
	return layer;
}

void info_layer_destroy(InfoLayer *layer)
{
	LayerData *data = (LayerData *)layer_get_data(layer->layer);
	for (int i = 0; i < 10; i++) {
		gbitmap_destroy(data->batteries[i]);
	}

	layer_destroy(layer->layer);
	free(layer);
}

Layer *info_layer_get_layer(InfoLayer *layer)
{
	return layer->layer;
}

void info_layer_show(InfoLayer *layer)
{
	LayerData *data = (LayerData *)layer_get_data(layer->layer);
	time_t now = time(NULL);

	if (now - data->tap_time > 3) {
		data->tap_time = now;
		return;
	}

	data->tap_time = now;
	if (data->app_timer != NULL) {
		app_timer_cancel(data->app_timer);
		data->app_timer = NULL;
	}

	layer_set_hidden(layer->layer, false);
	layer_mark_dirty(layer->layer);
	data->app_timer = app_timer_register(5000, hide_info, layer);
}

static void hide_info(void *bag)
{
	InfoLayer *layer = (InfoLayer *)bag;
	LayerData *data = (LayerData *)layer_get_data(layer->layer);

	if (data->app_timer != NULL) {
		app_timer_cancel(data->app_timer);
		data->app_timer = NULL;
	}

	layer_set_hidden(layer->layer, true);
}

void info_layer_hide(InfoLayer *layer)
{
	hide_info(layer);
}

void  info_layer_set_info(InfoLayer *layer, struct tm *tick_time, int battery_index)
{
	LayerData *data = (LayerData *)layer_get_data(layer->layer);
	data->battery_index = battery_index;
	strftime(data->date_text, sizeof(data->date_text), "%B %e", tick_time);
}
