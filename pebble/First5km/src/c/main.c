#define MAIN
#include "include.h"

static void init(void);
static void set_defaults(void);
static void deinit(void);

int main(void)
{
	init();
	if (multi_user) {
		view_user_init();
	} else {
		view_main_init();
	}
	app_event_loop();
	deinit();
}

static void init(void)
{
	small_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
	large_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
	set_defaults();
	multi_user = persist_read_int(P_MULTIUSER_KEY);
	selected_user = persist_read_int(P_SELECTEDUSER_KEY);

	for (int index = 0; index < NUMBER_OF_USERS; index++) {
		persist_read_data(P_COMPLETED_KEY + index, &user_profiles[index].language, sizeof(struct user_profile)); 
	}

	walking = gbitmap_create_with_resource(RESOURCE_ID_WALKING);
	running = gbitmap_create_with_resource(RESOURCE_ID_RUNNING);
	walking_home = gbitmap_create_with_resource(RESOURCE_ID_WALKING_HOME);
	running_home = gbitmap_create_with_resource(RESOURCE_ID_RUNNING_HOME);
	finish = gbitmap_create_with_resource(RESOURCE_ID_FINISH);
	paused = gbitmap_create_with_resource(RESOURCE_ID_PAUSED);
}

static void set_defaults(void)
{
	multi_user = 0;
	selected_user = 0;
	for (int index = 0; index < NUMBER_OF_USERS; index++) {
		user_profiles[index].language = LOCALE_LANGUAGE_ENGLISH;
		memset(user_profiles[index].completed, 0, MAX_DAYS / 8);
	}
}

static void deinit(void)
{
	gbitmap_destroy(paused);
	gbitmap_destroy(finish);
	gbitmap_destroy(running_home);
	gbitmap_destroy(walking_home);
	gbitmap_destroy(running);
	gbitmap_destroy(walking);

	persist_write_int(P_VERSION_KEY, 1);
	persist_write_int(P_MULTIUSER_KEY, multi_user);
	persist_write_int(P_SELECTEDUSER_KEY, selected_user);

	for (int index = 0; index < NUMBER_OF_USERS; index++) {
		persist_write_data(P_COMPLETED_KEY + index, &user_profiles[index].language, sizeof(struct user_profile)); 
	}
}
