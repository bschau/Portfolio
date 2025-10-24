#define MAIN
#include "include.h"

static void init(void);
static void deinit(void);

int main(void)
{
	init();
	view_main_init();
	app_event_loop();
	deinit();
}

static void init(void)
{
	large_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
	tiny_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
	leopard_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LEOPARD);
	rabbit_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RABBIT);
	turtle_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TURTLE);
	rounds_count = 1;
}

static void deinit(void)
{
	gbitmap_destroy(turtle_image);
	gbitmap_destroy(rabbit_image);
	gbitmap_destroy(leopard_image);
}
