#ifndef INCLUDE_H
#define INCLUDE_H 1
#include <pebble.h>

/* utils.c */
Window *window_new(void (*click_config_provider)(void *context), void (*load)(Window *window), void (*unload)(Window *window), void (*appear)(Window *window));

/* view_action.c */
void view_action_init(void);

/* view_main.c */
void view_main_init(void);

#ifdef MAIN
	GFont large_font, tiny_font;
	GBitmap *leopard_image, *rabbit_image, *turtle_image;
	int rounds_count;
#else
	extern GFont large_font, tiny_font;
	extern GBitmap *leopard_image, *rabbit_image, *turtle_image;
	extern int rounds_count;
#endif

#endif
