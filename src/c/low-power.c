#include <pebble.h>
#include "message_keys.auto.h"

// Persistent storage key
#define SETTINGS_KEY 1

// Settings structure
static bool s_show_date_info = true;  // Default to showing date info

static Window *s_window;
static TextLayer *s_hour_layer;
static TextLayer *s_minute_layer;
static TextLayer *s_weekday_layer;
static TextLayer *s_month_layer;
static TextLayer *s_day_layer;
static GFont s_time_font;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char hour_buffer[4];
  static char minute_buffer[5];
  static char weekday_buffer[10];
  static char month_buffer[10];
  static char day_buffer[4];

  strftime(hour_buffer, sizeof(hour_buffer), clock_is_24h_style() ? "%H" : "%I", tick_time);
  strftime(minute_buffer, sizeof(minute_buffer), ":%M", tick_time);
  strftime(weekday_buffer, sizeof(weekday_buffer), "%a", tick_time);
  strftime(month_buffer, sizeof(month_buffer), "%b", tick_time);
  strftime(day_buffer, sizeof(day_buffer), "%d", tick_time);

  text_layer_set_text(s_hour_layer, hour_buffer);
  text_layer_set_text(s_minute_layer, minute_buffer);
  
  // Show/hide date info based on settings
  if (s_show_date_info) {
    text_layer_set_text(s_weekday_layer, weekday_buffer);
    text_layer_set_text(s_month_layer, month_buffer);
    text_layer_set_text(s_day_layer, day_buffer);
    layer_set_hidden(text_layer_get_layer(s_weekday_layer), false);
    layer_set_hidden(text_layer_get_layer(s_month_layer), false);
    layer_set_hidden(text_layer_get_layer(s_day_layer), false);
  } else {
    layer_set_hidden(text_layer_get_layer(s_weekday_layer), true);
    layer_set_hidden(text_layer_get_layer(s_month_layer), true);
    layer_set_hidden(text_layer_get_layer(s_day_layer), true);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  // Use unobstructed bounds for Timeline Quick View support
  GRect bounds = layer_get_unobstructed_bounds(window_layer);

  window_set_background_color(window, GColorBlack);

  // Load custom font for larger time display
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_72));

  // Calculate proportional sizes - avoid hardcoded values per best practices
  int16_t padding = PBL_IF_ROUND_ELSE(bounds.size.w / 10, 5);
  int16_t date_row_height = 18;
  int16_t half_width = bounds.size.w / 2;
  int16_t date_start_y = PBL_IF_ROUND_ELSE(bounds.size.h / 6, 0);

  int16_t date_spacing = 14;  // Tighter spacing between date rows

  // Weekday layer - top left
  s_weekday_layer = text_layer_create(GRect(
    padding, 
    date_start_y, 
    half_width - padding, 
    date_row_height
  ));
  text_layer_set_background_color(s_weekday_layer, GColorClear);
  text_layer_set_text_color(s_weekday_layer, GColorWhite);
  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
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
  text_layer_set_text_color(s_month_layer, GColorWhite);
  text_layer_set_font(s_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
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
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_day_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_day_layer));

  // Hour layer - bottom right, above minutes (larger custom font 72px)
  s_hour_layer = text_layer_create(GRect(
    0, 
    bounds.size.h / 2 - 80 + PBL_IF_ROUND_ELSE(10, 0), 
    bounds.size.w - padding, 
    80
  ));
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorWhite);
  text_layer_set_font(s_hour_layer, s_time_font);
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_hour_layer));

  // Minute layer - bottom right (wider to fit colon, larger custom font 72px)
  s_minute_layer = text_layer_create(GRect(
    0, 
    bounds.size.h / 2 - 5 + PBL_IF_ROUND_ELSE(5, 0), 
    bounds.size.w - padding, 
    80
  ));
  text_layer_set_background_color(s_minute_layer, GColorClear);
  text_layer_set_text_color(s_minute_layer, GColorWhite);
  text_layer_set_font(s_minute_layer, s_time_font);
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));

  update_time();
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_minute_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_month_layer);
  text_layer_destroy(s_day_layer);
  fonts_unload_custom_font(s_time_font);
}

// Load settings from persistent storage
static void load_settings() {
  if (persist_exists(SETTINGS_KEY)) {
    s_show_date_info = persist_read_bool(SETTINGS_KEY);
  }
}

// Save settings to persistent storage
static void save_settings() {
  persist_write_bool(SETTINGS_KEY, s_show_date_info);
}

// Handle incoming messages from phone (settings)
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *show_date_tuple = dict_find(iterator, MESSAGE_KEY_ShowDateInfo);
  if (show_date_tuple) {
    s_show_date_info = show_date_tuple->value->int32 == 1;
    save_settings();
    update_time();  // Update display immediately
  }
}

// Handle dropped messages
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped! Reason: %d", reason);
}

// Handle failed message sends
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! Reason: %d", reason);
}

// Handle successful message sends
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void prv_init(void) {
  // Load settings
  load_settings();
  
  // Register AppMessage callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
