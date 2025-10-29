#include "ray.h"

static void set_beam(uint8_t *beam, int index);
static void beam_update(struct Layer *layer, GContext *ctx);
static void beam_app_timer_callback(void *data);

static uint8_t band_2[] = { 8, 8, 8, 0, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8 };
static uint8_t band_1[] = { 8, 8, 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 6, 7, 8 };
static uint8_t band_0[] = { 8, 0, 1, 2, 3, 3, 3, 3, 4, 5, 5, 5, 6, 7, 8 };

static BitmapLayer *ray_layers[3] = { NULL, NULL, NULL };
static Layer *layers[3] = { NULL, NULL, NULL };
static uint8_t *beams[3] = { NULL, NULL, NULL };
static unsigned int anim_count = 0;
static AppTimer *app_timer;

void beam_init(Window *window) {
	GRect rectangle = { .origin = { 0, 32 }, .size = { 144, 5 } };
	Layer *window_layer = window_get_root_layer(window);
	BitmapLayer *bitmap;

	for (int i = 0; i < 3; i++) {
		bitmap = bitmap_layer_create(rectangle);
		ray_layers[i] = bitmap;

		bitmap_layer_set_background_color(bitmap, GColorBlack);
		layers[i] = bitmap_layer_get_layer(bitmap);
		layer_set_update_proc(layers[i], beam_update);
		layer_set_hidden(layers[i], true);

		rectangle.origin.y += 42;
	}

	for (int i = 0; i < 3; i++) {
		layer_add_child(window_layer, layers[i]);
	}

	beam_bands[0] = beam_red;
	beam_bands[1] = beam_purple;
	beam_bands[2] = beam_green;
	beam_bands[3] = beam_brown;
	beam_bands[4] = beam_gray;
	beam_bands[5] = beam_blue;
	beam_bands[6] = beam_yellow;
	beam_bands[7] = beam_pink;
	beam_bands[8] = beam_turquoise;
}

void beam_deinit(void)
{
	if (app_timer != NULL) {
		app_timer_cancel(app_timer);
		app_timer = NULL;
	}

	for (int i = 0; i < 3; i++) {
		bitmap_layer_destroy(ray_layers[i]);
	}
}

void beam_reset_animation(uint8_t *beam1, uint8_t *beam2, uint8_t *beam3)
{
	anim_count = 0;
	set_beam(beam1, 0);
	set_beam(beam2, 1);
	set_beam(beam3, 2);
	app_timer = app_timer_register(100, beam_app_timer_callback, NULL);
}

void beam_clear(void)
{
	set_beam(NULL, 0);
	set_beam(NULL, 1);
	set_beam(NULL, 2);
}

void beam_shift(void)
{
	uint8_t *first = beam_bands[0];

	for (int i = 0; i < 8; i++) {
		beam_bands[i] = beam_bands[i + 1];
	}

	beam_bands[8] = first;
}

static void set_beam(uint8_t *beam, int index)
{
	beams[index] = beam;

	if (beam == NULL) {
		layer_set_hidden(layers[index], true);
		return;
	}

	layer_set_hidden(layers[index], false);
	layer_mark_dirty(layers[index]);
}

static void beam_update(struct Layer *layer, GContext *ctx)
{
	GPoint from = { 0, 2 };
	GPoint to = { 143, 2 };
	uint8_t *beam;
	uint8_t color;

	if (anim_count >= sizeof(band_0)) {
		return;
	}

	if (layer == layers[0]) {
		beam = beams[0];
	} else if (layer == layers[1]) {
		beam = beams[1];
	} else if (layer == layers[2]) {
		beam = beams[2];
	} else {
		return;
	}

	graphics_context_set_stroke_width(ctx, 1);

	color = beam[band_0[anim_count]];
	graphics_context_set_stroke_color(ctx, (GColor){.argb = color});
	graphics_draw_line(ctx, from, to);

	from.y--;
	to.y--;

	color = beam[band_1[anim_count]];
	graphics_context_set_stroke_color(ctx, (GColor){.argb = color});
	graphics_draw_line(ctx, from, to);

	from.y += 2;
	to.y += 2;
	graphics_draw_line(ctx, from, to);

	from.y++;
	to.y++;
	color = beam[band_2[anim_count]];
	graphics_context_set_stroke_color(ctx, (GColor){.argb = color});
	graphics_draw_line(ctx, from, to);

	from.y -= 4;
	to.y -= 4;
	graphics_draw_line(ctx, from, to);
}

static void beam_app_timer_callback(void *data)
{
	anim_count++;

	layer_mark_dirty(layers[0]);
	layer_mark_dirty(layers[1]);
	layer_mark_dirty(layers[2]);

	if (anim_count > 14) {
		app_timer = NULL;
	} else {
		int ms = anim_count == 7 ? 2000 : 100;
		app_timer = app_timer_register(ms, beam_app_timer_callback, NULL);
	}
}
