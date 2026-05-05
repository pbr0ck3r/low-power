#include <pebble.h>

static Window *s_window;
static TextLayer *s_hour_layer;
static TextLayer *s_minute_layer;
static TextLayer *s_seconds_layer;
static TextLayer *s_weekday_layer;
static TextLayer *s_month_layer;
static TextLayer *s_day_layer;
static GFont s_time_font;
static bool s_show_date = true; // Default to showing date
static bool s_show_seconds = false; // Default to hiding seconds
static GColor s_text_color;
static GColor s_background_color;

// Helper function to convert hex color string to GColor
static GColor color_from_hex(int hex) {
  return GColorFromHEX(hex);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char hour_buffer[4];
  static char minute_buffer[5];
  static char seconds_buffer[4];
  static char weekday_buffer[10];
  static char month_buffer[10];
  static char day_buffer[4];

  strftime(hour_buffer, sizeof(hour_buffer), clock_is_24h_style() ? "%H" : "%I", tick_time);
  strftime(minute_buffer, sizeof(minute_buffer), ":%M", tick_time);
  strftime(seconds_buffer, sizeof(seconds_buffer), "%S", tick_time);
  strftime(weekday_buffer, sizeof(weekday_buffer), "%a", tick_time);
  strftime(month_buffer, sizeof(month_buffer), "%b", tick_time);
  strftime(day_buffer, sizeof(day_buffer), "%d", tick_time);

  text_layer_set_text(s_hour_layer, hour_buffer);
  text_layer_set_text(s_minute_layer, minute_buffer);
  
  // Only update seconds layer if it should be shown
  if (s_show_seconds) {
    text_layer_set_text(s_seconds_layer, seconds_buffer);
  }
  
  // Only update date layers if they should be shown
  if (s_show_date) {
    text_layer_set_text(s_weekday_layer, weekday_buffer);
    text_layer_set_text(s_month_layer, month_buffer);
    text_layer_set_text(s_day_layer, day_buffer);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void update_date_visibility() {
  // Show or hide the date layers based on the setting
  layer_set_hidden(text_layer_get_layer(s_weekday_layer), !s_show_date);
  layer_set_hidden(text_layer_get_layer(s_month_layer), !s_show_date);
  layer_set_hidden(text_layer_get_layer(s_day_layer), !s_show_date);
}

static void update_seconds_visibility() {
  // Show or hide the seconds layer based on the setting
  layer_set_hidden(text_layer_get_layer(s_seconds_layer), !s_show_seconds);
}

static void update_tick_timer() {
  // Unsubscribe from existing timer
  tick_timer_service_unsubscribe();
  
  // Subscribe based on whether we need seconds updates
  if (s_show_seconds) {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }
}

static void update_colors() {
  // Update background color
  window_set_background_color(s_window, s_background_color);
  
  // Update all text layers with the selected text color
  text_layer_set_text_color(s_hour_layer, s_text_color);
  text_layer_set_text_color(s_minute_layer, s_text_color);
  text_layer_set_text_color(s_seconds_layer, s_text_color);
  text_layer_set_text_color(s_weekday_layer, s_text_color);
  text_layer_set_text_color(s_month_layer, s_text_color);
  text_layer_set_text_color(s_day_layer, s_text_color);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Read boolean preferences
  Tuple *show_date_t = dict_find(iter, MESSAGE_KEY_ShowDate);
  if (show_date_t) {
    s_show_date = show_date_t->value->int32 == 1;
    persist_write_bool(MESSAGE_KEY_ShowDate, s_show_date);
    update_date_visibility();
    update_time();
  }
  
  Tuple *show_seconds_t = dict_find(iter, MESSAGE_KEY_ShowSeconds);
  if (show_seconds_t) {
    s_show_seconds = show_seconds_t->value->int32 == 1;
    persist_write_bool(MESSAGE_KEY_ShowSeconds, s_show_seconds);
    update_seconds_visibility();
    update_tick_timer(); // Update timer frequency based on seconds visibility
    update_time();
  }
  
  // Read color preferences
  Tuple *text_color_t = dict_find(iter, MESSAGE_KEY_TextColor);
  if (text_color_t) {
    int text_color_hex = text_color_t->value->int32;
    s_text_color = color_from_hex(text_color_hex);
    persist_write_int(MESSAGE_KEY_TextColor, text_color_hex);
    update_colors();
  }
  
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    int bg_color_hex = bg_color_t->value->int32;
    s_background_color = color_from_hex(bg_color_hex);
    persist_write_int(MESSAGE_KEY_BackgroundColor, bg_color_hex);
    update_colors();
  }
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  // Use unobstructed bounds for Timeline Quick View support
  GRect bounds = layer_get_unobstructed_bounds(window_layer);

  // Set background color (will be updated with saved color preference)
  window_set_background_color(window, s_background_color);

  // Load custom font - larger on Pebble Time 2 (emery) for its bigger screen
#if defined(PBL_PLATFORM_EMERY)
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_74));
  int16_t time_layer_height = 84;
  int16_t date_row_height = 22;
  int16_t date_spacing = 18;
  int16_t seconds_height = 28;
  GFont date_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GFont seconds_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
#else
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_72));
  int16_t time_layer_height = 80;
  int16_t date_row_height = 18;
  int16_t date_spacing = 14;
  int16_t seconds_height = 24;
  GFont date_font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  GFont seconds_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
#endif

  int16_t padding = PBL_IF_ROUND_ELSE(bounds.size.w / 10, 5);
  int16_t half_width = bounds.size.w / 2;
  int16_t date_start_y = PBL_IF_ROUND_ELSE(bounds.size.h / 6, 0);

  // Weekday layer - top left
  s_weekday_layer = text_layer_create(GRect(
    padding, 
    date_start_y, 
    half_width - padding, 
    date_row_height
  ));
  text_layer_set_background_color(s_weekday_layer, GColorClear);
  text_layer_set_text_color(s_weekday_layer, s_text_color);
  text_layer_set_font(s_weekday_layer, date_font);
  text_layer_set_text_alignment(s_weekday_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_weekday_layer));

  // Month layer - below weekday
  s_month_layer = text_layer_create(GRect(
    padding,
    date_start_y + date_spacing,
    half_width - padding,
    date_row_height
  ));
  text_layer_set_background_color(s_month_layer, GColorClear);
  text_layer_set_text_color(s_month_layer, s_text_color);
  text_layer_set_font(s_month_layer, date_font);
  text_layer_set_text_alignment(s_month_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_month_layer));

  // Day layer - below month
  s_day_layer = text_layer_create(GRect(
    padding,
    date_start_y + date_spacing * 2,
    half_width - padding,
    date_row_height
  ));
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, s_text_color);
  text_layer_set_font(s_day_layer, date_font);
  text_layer_set_text_alignment(s_day_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_day_layer));

  // Seconds layer - middle left
  s_seconds_layer = text_layer_create(GRect(
    padding,
    bounds.size.h / 2 - seconds_height / 2,
    half_width - padding,
    seconds_height
  ));
  text_layer_set_background_color(s_seconds_layer, GColorClear);
  text_layer_set_text_color(s_seconds_layer, s_text_color);
  text_layer_set_font(s_seconds_layer, seconds_font);
  text_layer_set_text_alignment(s_seconds_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_seconds_layer));

  // Anchor time to bottom of screen
  int16_t time_bottom_padding = 5;
  int16_t minute_y = bounds.size.h - time_layer_height - time_bottom_padding;
  int16_t hour_y = minute_y - time_layer_height;

  // Hour layer - above minutes
  s_hour_layer = text_layer_create(GRect(
    0,
    hour_y,
    bounds.size.w - padding,
    time_layer_height
  ));
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, s_text_color);
  text_layer_set_font(s_hour_layer, s_time_font);
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_hour_layer));

  // Minute layer - bottom right (wider to fit colon)
  s_minute_layer = text_layer_create(GRect(
    0,
    minute_y,
    bounds.size.w - padding,
    time_layer_height
  ));
  text_layer_set_background_color(s_minute_layer, GColorClear);
  text_layer_set_text_color(s_minute_layer, s_text_color);
  text_layer_set_font(s_minute_layer, s_time_font);
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));

  // Apply the visibility settings
  update_date_visibility();
  update_seconds_visibility();
  update_time();
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_minute_layer);
  text_layer_destroy(s_seconds_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_month_layer);
  text_layer_destroy(s_day_layer);
  fonts_unload_custom_font(s_time_font);
}

static void prv_init(void) {
  // Load saved settings
  if (persist_exists(MESSAGE_KEY_ShowDate)) {
    s_show_date = persist_read_bool(MESSAGE_KEY_ShowDate);
  }
  
  if (persist_exists(MESSAGE_KEY_ShowSeconds)) {
    s_show_seconds = persist_read_bool(MESSAGE_KEY_ShowSeconds);
  }
  
  // Load saved colors or use defaults (white text on black background)
  if (persist_exists(MESSAGE_KEY_TextColor)) {
    s_text_color = color_from_hex(persist_read_int(MESSAGE_KEY_TextColor));
  } else {
    s_text_color = GColorWhite; // White
  }
  
  if (persist_exists(MESSAGE_KEY_BackgroundColor)) {
    s_background_color = color_from_hex(persist_read_int(MESSAGE_KEY_BackgroundColor));
  } else {
    s_background_color = GColorBlack; // Black
  }
  
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);

  // Subscribe to timer based on seconds visibility
  update_tick_timer();
  
  // Register AppMessage handlers
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 128);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
