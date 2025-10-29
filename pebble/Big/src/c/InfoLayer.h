#ifndef INFOLAYER_H
#define INFOLAYER_H

#include <pebble.h>

typedef struct {
	Layer *layer;
	int width;
} InfoLayer;

InfoLayer *info_layer_create(int width, int height);
void info_layer_destroy(InfoLayer *layer);
Layer *info_layer_get_layer(InfoLayer *layer);
void info_layer_show(InfoLayer *layer);
void info_layer_hide(InfoLayer *layer);
void  info_layer_set_info(InfoLayer *layer, struct tm *tick_time, int battery_index);

#endif