#include "lights.h"

#define BULBS (60)
#define RED (0)
#define GREEN (1)
#define BLUE (2)
#define YELLOW (3)
#define PINK (4)
#define TURQUOISE (5)
#define WIDTH (133)
#define HEIGHT (151)
#define BULB_WIDTH (7)
#define BULB_HEIGHT (7)

typedef struct {
	uint8_t current[BULBS];
	uint8_t next[BULBS];
	GBitmap *canvas;
	GBitmap *lights_on[6];
	GBitmap *lights_off[6];
} LayerData;

static void update_proc(Layer *layer, GContext *ctx);

static uint8_t x_pos[BULBS] = {
	63, 72, 81, 90, 99, 108, 117, 126,
	126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
	126, 126, 126,
	117, 108, 99, 90, 81, 72, 63, 54, 45, 36, 27, 18, 9, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 9, 18, 27, 36, 45, 54
};

static uint8_t y_pos[BULBS] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117,
	126, 135, 144,
	144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144,
	135, 126, 117, 108, 99, 90, 81, 72, 63, 54, 45, 36, 27, 18, 9,
	0, 0, 0, 0, 0, 0, 0
};

static uint8_t pattern[BULBS] = {
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE,
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE,
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE,
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE,
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE,
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE,
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE,
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE,
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE,
	RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE
};

static void update_proc(Layer *layer, GContext *ctx)
{
	LayerData *data = (LayerData *)layer_get_data(layer);
	GRect viewport = { .origin = { 0, 0 }, .size = { WIDTH, HEIGHT } };

	uint8_t *canvas = gbitmap_get_data(data->canvas);

	for (int i = 0; i < BULBS; i++) {
		if (data->current[i] == data->next[i]) {
			continue;
		}

		int bulb = pattern[i];
		GBitmap *bulb_bitmap = data->next[i] ? data->lights_on[bulb] : data->lights_off[bulb];
		uint8_t *bitmap = gbitmap_get_data(bulb_bitmap);
		int bitmap_index = 0;
		int line = (y_pos[i] * WIDTH) + x_pos[i];

		for (int row = 0; row < BULB_HEIGHT; row++) {
			int index = line;
			for (int column = 0; column < BULB_WIDTH; column++) {
				canvas[index++] = bitmap[bitmap_index++];
			}
			line += WIDTH;
		}
	}

	graphics_draw_bitmap_in_rect(ctx, data->canvas, viewport);
	memmove(data->current, data->next, BULBS);
}

LightsLayer *lights_layer_create(GPoint origin)
{
	LightsLayer *layer = malloc(sizeof(LightsLayer));
	layer->layer = layer_create_with_data(GRect(origin.x, origin.y, WIDTH, HEIGHT), sizeof(LayerData));
	layer_set_update_proc(layer->layer, (LayerUpdateProc)update_proc);
  
	LayerData *data = (LayerData *)layer_get_data(layer->layer);
	memset(data->current, 128, BULBS);
	memset(data->next, 0, BULBS);

	data->canvas = gbitmap_create_with_resource(RESOURCE_ID_LIGHTS_CANVAS);
	data->lights_on[RED] = gbitmap_create_with_resource(RESOURCE_ID_RED_ON);
	data->lights_on[GREEN] = gbitmap_create_with_resource(RESOURCE_ID_GREEN_ON);
	data->lights_on[BLUE] = gbitmap_create_with_resource(RESOURCE_ID_BLUE_ON);
	data->lights_on[YELLOW] = gbitmap_create_with_resource(RESOURCE_ID_YELLOW_ON);
	data->lights_on[PINK] = gbitmap_create_with_resource(RESOURCE_ID_PINK_ON);
	data->lights_on[TURQUOISE] = gbitmap_create_with_resource(RESOURCE_ID_TURQUOISE_ON);
	data->lights_off[RED] = gbitmap_create_with_resource(RESOURCE_ID_RED_OFF);
	data->lights_off[GREEN] = gbitmap_create_with_resource(RESOURCE_ID_GREEN_OFF);
	data->lights_off[BLUE] = gbitmap_create_with_resource(RESOURCE_ID_BLUE_OFF);
	data->lights_off[YELLOW] = gbitmap_create_with_resource(RESOURCE_ID_YELLOW_OFF);
	data->lights_off[PINK] = gbitmap_create_with_resource(RESOURCE_ID_PINK_OFF);
	data->lights_off[TURQUOISE] = gbitmap_create_with_resource(RESOURCE_ID_TURQUOISE_OFF);

	return layer;
}

void lights_layer_destroy(LightsLayer *layer)
{
	LayerData *data = (LayerData *)layer_get_data(layer->layer);
	for (int i = 0; i < 6; i++) {
		gbitmap_destroy(data->lights_on[i]);
		gbitmap_destroy(data->lights_off[i]);
	}

	layer_destroy(layer->layer);
	free(layer);
}

Layer *lights_layer_get_layer(LightsLayer *layer)
{
	return layer->layer;
}

void lights_layer_reset_bulbs(LightsLayer *layer)
{
	LayerData *data = (LayerData *)layer_get_data(layer->layer);
	memset(data->next, 0, BULBS);
}

void lights_layer_set_active(LightsLayer *layer, int active)
{
	if (active < 1) {
		active = 1;
	} else if (active > BULBS) {
		active = BULBS;
	}

	LayerData *data = (LayerData *)layer_get_data(layer->layer);
	memset(data->next, 1, active);
	layer_mark_dirty(layer->layer);
}
