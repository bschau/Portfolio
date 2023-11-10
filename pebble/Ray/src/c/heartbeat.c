#include "ray.h"

static void animation_stopped_handler(Animation *animation, bool finished, void *context);
static void hide_layers(bool hide);

#define POS_X (144)
#define POS_Y (34)
#define WIDTH (48)
#define HEIGHT (80)

static GRect start_frame = { .origin = { POS_X, POS_Y }, .size = { WIDTH, HEIGHT } };
static GRect end_frame = { .origin = { -WIDTH, POS_Y }, .size = { WIDTH, HEIGHT } };
static BitmapLayer *ray_layer;
static BitmapLayer *overlay_layer;
static PropertyAnimation *animation;
static GBitmap *rays[9] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static GBitmap *overlay;
static int color;

void heartbeat_init(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	color = -1;

	rays[0] = gbitmap_create_with_resource(RESOURCE_ID_RED_RAY);
	rays[1] = gbitmap_create_with_resource(RESOURCE_ID_PURPLE_RAY);
	rays[2] = gbitmap_create_with_resource(RESOURCE_ID_GREEN_RAY);
	rays[3] = gbitmap_create_with_resource(RESOURCE_ID_BROWN_RAY);
	rays[4] = gbitmap_create_with_resource(RESOURCE_ID_GRAY_RAY);
	rays[5] = gbitmap_create_with_resource(RESOURCE_ID_BLUE_RAY);
	rays[6] = gbitmap_create_with_resource(RESOURCE_ID_YELLOW_RAY);
	rays[7] = gbitmap_create_with_resource(RESOURCE_ID_PINK_RAY);
	rays[8] = gbitmap_create_with_resource(RESOURCE_ID_TURQUOISE_RAY);
	overlay = gbitmap_create_with_resource(RESOURCE_ID_OVERLAY);

	ray_layer = bitmap_layer_create(start_frame);
	layer_add_child(window_layer, bitmap_layer_get_layer(ray_layer));

	overlay_layer = bitmap_layer_create((GRect) { .origin = { 0, POS_Y }, .size = { 144, HEIGHT } });
	bitmap_layer_set_bitmap(overlay_layer, overlay);
	bitmap_layer_set_compositing_mode(overlay_layer, GCompOpSet);
	layer_add_child(window_layer, bitmap_layer_get_layer(overlay_layer));
	hide_layers(true);
}

void heartbeat_deinit(void)
{
	if (animation != NULL) {
		animation_unschedule((Animation *)animation);
		animation_destroy((Animation *)animation);
	}

	bitmap_layer_destroy(overlay_layer);
	bitmap_layer_destroy(ray_layer);

	for (int i = 0; i < 9; i++) {
		gbitmap_destroy(rays[i]);
	}

	gbitmap_destroy(overlay);
}

void heartbeat_reset_animation(void)
{
	color++;
	if (color > 8) {
		color = 0;
	}

	bitmap_layer_set_bitmap(ray_layer, rays[color]);

	hide_layers(false);

	animation = property_animation_create_layer_frame(bitmap_layer_get_layer(ray_layer), &start_frame, &end_frame);
	animation_set_duration((Animation *)animation, 2000);
	animation_set_handlers((Animation *)animation, (AnimationHandlers) {
		.stopped = animation_stopped_handler
	}, NULL);
	animation_schedule((Animation *)animation);
}

static void animation_stopped_handler(Animation *animation, bool finished, void *context)
{
	if (finished) {
		hide_layers(true);
	}
}

static void hide_layers(bool hide)
{
	layer_set_hidden(bitmap_layer_get_layer(ray_layer), hide);
	layer_set_hidden(bitmap_layer_get_layer(overlay_layer), hide);
}
