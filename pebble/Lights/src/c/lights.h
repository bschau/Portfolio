#ifndef LIGHTS_H
#define LIGHTS_H

#include <pebble.h>

typedef struct {
	Layer *layer;
} LightsLayer;

LightsLayer *lights_layer_create(GPoint origin);
void lights_layer_destroy(LightsLayer *layer);
Layer *lights_layer_get_layer(LightsLayer *layer);
void lights_layer_reset_bulbs(LightsLayer *layer);
void lights_layer_set_active(LightsLayer *layer, int active);

#endif
