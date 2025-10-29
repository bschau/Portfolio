#include <pebble.h>

#define StorageKeyTime (1)
#define StorageKeySteps (2)
#define StorageKeyCalories (3)

static void load_environment(void);
static void save_environment(void);
static void init(void);
static void click_config_provider(void *context);
static void deinit(void);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void down_click_handler(ClickRecognizerRef recognizer, void *context);
static void update_info(void);
static void enable_tick_service(bool enabled);
static void handle_tick(struct tm *tick_time, TimeUnits units_changed);

typedef enum {
	Idle = 0,
	Monitoring,
	Paused
} State;

static Window *window;
static StatusBarLayer *status_bar;
static ActionBarLayer *action_bar;
static GBitmap *play_button, *stop_button, *pause_button;
static GBitmap *icon_images[3] = { NULL, NULL, NULL };
static BitmapLayer *icons[3] = { NULL, NULL, NULL };
static TextLayer *texts[3] = { NULL, NULL, NULL };
static TextLayer *again_to_stop;
static Layer *text_layers[3] = { NULL, NULL, NULL };
static GFont font, font_small;
static char clock_buffer[12];
static char steps_buffer[12];
static char calories_buffer[12];
static int32_t p_clock, p_steps, p_calories;
static int32_t l_steps, l_calories;
static State state;
static time_t clear_again;

int main(void)
{
	load_environment();
	init();
	app_event_loop();
	deinit();
	save_environment();
}

static void load_environment(void)
{
	p_clock = persist_read_int(StorageKeyTime);
	p_steps = persist_read_int(StorageKeySteps);
	p_calories = persist_read_int(StorageKeyCalories);
	
#if 0
	p_clock = 60 + 54;
	p_steps = 54;
	p_calories = 6;
#endif
}

static void save_environment(void)
{
	persist_write_int(StorageKeyTime, p_clock);
	persist_write_int(StorageKeySteps, p_steps);
	persist_write_int(StorageKeyCalories, p_calories);
}

static void init(void)
{
	state = Idle;
	
	play_button = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLAY);
	stop_button = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STOP);
	pause_button = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAUSE);
	icon_images[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CLOCK);
	icon_images[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WALK);
	icon_images[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BURN);

	font = fonts_get_system_font(FONT_KEY_GOTHIC_28);
	font_small = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);

	window = window_create();
	Layer *window_layer = window_get_root_layer(window);
	window_set_background_color(window, GColorWhite);
	GRect window_bounds = layer_get_bounds(window_layer);

	action_bar = action_bar_layer_create();
	action_bar_layer_set_click_config_provider(action_bar, click_config_provider);
#if PBL_COLOR
	action_bar_layer_set_background_color(action_bar, GColorLightGray);
#else
	action_bar_layer_set_background_color(action_bar, GColorBlack);
#endif	
	action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, play_button, true);
	action_bar_layer_add_to_window(action_bar, window);

	GRect frame = GRect(0, 28, 28, 30);
	GRect text_frame = GRect(31, frame.origin.y - 3, 144 - text_frame.origin.x - ACTION_BAR_WIDTH, 30);
#if PBL_ROUND
	frame.origin.x += 20;
	frame.origin.y += 8;
	text_frame.origin.x += 24;
	text_frame.origin.y += 8;
#endif
	
	for (int i = 0; i < 3; i++) {
		icons[i] = bitmap_layer_create(frame);
		bitmap_layer_set_background_color(icons[i], GColorWhite);
		bitmap_layer_set_bitmap(icons[i], icon_images[i]);
		Layer *bitmap_layer = bitmap_layer_get_layer(icons[i]);
		layer_add_child(window_layer, bitmap_layer);

		texts[i] = text_layer_create(text_frame);
		text_layer_set_background_color(texts[i], GColorWhite);
		text_layer_set_text_color(texts[i], GColorBlack);
		text_layer_set_font(texts[i], font);
		text_layer_set_text_alignment(texts[i], GTextAlignmentLeft);
		Layer *text_layer = text_layer_get_layer(texts[i]);
		layer_add_child(window_layer, text_layer);
		text_layers[i] = text_layer;

		frame.origin.y += 40;
		text_frame.origin.y += 40;
	}

	text_layer_set_text(texts[0], clock_buffer);
	text_layer_set_text(texts[1], steps_buffer);
	text_layer_set_text(texts[2], calories_buffer);
	
#if PBL_ROUND
	frame = GRect(0, 0, window_bounds.size.w, STATUS_BAR_LAYER_HEIGHT);
#else
	frame = GRect(0, 0, window_bounds.size.w - ACTION_BAR_WIDTH, STATUS_BAR_LAYER_HEIGHT);
#endif
	
	status_bar = status_bar_layer_create();
	Layer *status_bar_layer = status_bar_layer_get_layer(status_bar);
	layer_set_frame(status_bar_layer, frame);
#if PBL_COLOR
	status_bar_layer_set_colors(status_bar, GColorLightGray, GColorBlack);
#else
	status_bar_layer_set_colors(status_bar, GColorBlack, GColorWhite);
#endif
	layer_add_child(window_layer, status_bar_layer);

	update_info();
	
#if PBL_ROUND
	frame = GRect((window_bounds.size.w - 114) / 2, 148, 114, 20);
#else
	frame = GRect(0, 148, 114, 20);
#endif
	again_to_stop = text_layer_create(frame);
	text_layer_set_background_color(again_to_stop, GColorWhite);
	text_layer_set_text_color(again_to_stop, GColorBlack);
	text_layer_set_font(again_to_stop, font_small);
	text_layer_set_text_alignment(again_to_stop, GTextAlignmentCenter);
	text_layer_set_text(again_to_stop, "Again, to stop");
	Layer *text_layer = text_layer_get_layer(again_to_stop);
	layer_set_hidden(text_layer, true);
	layer_add_child(window_layer, text_layer);

	window_stack_push(window, true);
}

static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_click_handler);
}

static void deinit(void)
{
	enable_tick_service(false);
	
	text_layer_destroy(again_to_stop);
	for (int i = 0; i < 3; i++) {
		bitmap_layer_destroy(icons[i]);
		text_layer_destroy(texts[i]);
	}

	action_bar_layer_destroy(action_bar);
	status_bar_layer_destroy(status_bar);
	window_destroy(window);

	gbitmap_destroy(pause_button);
	gbitmap_destroy(stop_button);
	gbitmap_destroy(play_button);

	for (int i = 0; i < 3; i++) {
		gbitmap_destroy(icon_images[i]);
	}
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (state == Idle) {
		clear_again = 0;
		p_clock = 0;
		p_steps = 0;
		p_calories = 0;
		update_info();
		l_steps = health_service_sum_today(HealthMetricStepCount);
		l_calories = health_service_sum_today(HealthMetricActiveKCalories);
		enable_tick_service(true);
		action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, pause_button, true);
		action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, stop_button, true);
		state = Monitoring;
		return;
	}
	
	if (state == Monitoring) {
		state = Paused;
		enable_tick_service(false);
		action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, play_button, true);
		return;
	}
	
	state = Monitoring;
	enable_tick_service(true);
	action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, pause_button, true);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (state == Idle) {
		return;
	}
	
	Layer *text_layer = text_layer_get_layer(again_to_stop);
	if (clear_again == 0) {
		layer_set_hidden(text_layer, false);
		clear_again = time(NULL) + 5;
		return;
	}
	
	layer_set_hidden(text_layer, true);
	action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, play_button, true);
	action_bar_layer_clear_icon(action_bar, BUTTON_ID_DOWN);
	enable_tick_service(false);
	state = Idle;
}

static void update_info(void)
{
	char *p;
	int h, m;
	
	snprintf(steps_buffer, sizeof(steps_buffer), "%lu", (unsigned long int)p_steps);
	snprintf(calories_buffer, sizeof(calories_buffer), "%lu", (unsigned long int)p_calories);

	if (p_clock == 0) {
		strcpy(clock_buffer, "00:00");
	} else {
		p = clock_buffer,
		h = p_clock / 3600;
		if (h > 99) {
			strcpy(clock_buffer, "--:--:--");
			return;
		} else if (h > 9) {
			*p++ = (h / 10) + '0';
			*p++ = (h % 10) + '0';
			*p++ = ':';
		} else if (h > 0) {
			*p++ = h + '0';
			*p++ = ':';
		}
		
		m = (p_clock % 3600) / 60;
		*p++ = (m / 10) + '0';
		*p++ = (m % 10) + '0';
		*p++ = ':';
		
		m = (p_clock % 3600) % 60;
		*p++ = (m / 10) + '0';
		*p++ = (m % 10) + '0';
		*p = 0;
	}
}

static void enable_tick_service(bool enabled)
{
	if (enabled) {
		tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
		return;
	}
	
	tick_timer_service_unsubscribe();
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
	if (clear_again != 0) {
		time_t now = mktime(tick_time);
		if (now > clear_again) {
			Layer *text_layer = text_layer_get_layer(again_to_stop);
			layer_set_hidden(text_layer, true);
			clear_again = 0;
		}
	}
	
	if (state != Monitoring) {
		return;
	}
	
	p_clock++;
	
	int32_t value = health_service_sum_today(HealthMetricStepCount);
	p_steps += value - l_steps;
	l_steps = value;
	
	value = health_service_sum_today(HealthMetricActiveKCalories);
	p_calories += value - l_calories;
	l_calories = value;
	
	update_info();
	
	layer_mark_dirty(text_layers[0]);
	layer_mark_dirty(text_layers[1]);
	layer_mark_dirty(text_layers[2]);
}
