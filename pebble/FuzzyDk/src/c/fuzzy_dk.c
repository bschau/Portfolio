#define MAIN
#include "include.h"

static void init(void);
static void deinit(void);

int main(void)
{
	init();
	app_event_loop();
	deinit();
}

static void init(void)
{
	info_window_init();
	main_window_init();
}

static void deinit(void)
{
	main_window_deinit();
	info_window_deinit();
}
