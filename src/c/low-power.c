#include <pebble.h>
#include "fctx.h"
#include "ffont.h"

static Window *s_window;
static Layer *s_main_layer;
static FFont *s_font;
static bool s_show_date = true;
static bool s_show_seconds = false;
static GColor s_text_color;
static GColor s_background_color;

static char s_hour_buffer[4] = "--";
static char s_minute_buffer[5] = ":--";
static char s_weekday_buffer[10] = "";
static char s_month_buffer[10] = "";
static char s_day_buffer[4] = "";
static char s_seconds_buffer[4] = "";

static GColor color_from_hex(int hex) {
  return GColorFromHEX(hex);
}

static void main_layer_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  FContext fctx;
  fctx_init_context(&fctx, ctx);
  fctx_set_fill_color(&fctx, s_text_color);

  // Time: two large right-aligned digits stacked at bottom
  int16_t em_height = (bounds.size.h - 10) / 2;
  fctx_set_text_em_height(&fctx, s_font, em_height);
  bool is_large = bounds.size.h >= 200;
  int16_t right_pad = is_large ? bounds.size.w / 20 : 0;
  int16_t bottom_pad = is_large ? bounds.size.h / 20 : 5;
  int16_t minute_y = bounds.size.h - bottom_pad;
  int16_t hour_y = minute_y - em_height;

  fctx_begin_fill(&fctx);
  fctx_set_offset(&fctx, FPointI(bounds.size.w - right_pad, hour_y));
  fctx_draw_string(&fctx, s_hour_buffer, s_font, GTextAlignmentRight, FTextAnchorBaseline);
  fctx_set_offset(&fctx, FPointI(bounds.size.w - right_pad, minute_y));
  fctx_draw_string(&fctx, s_minute_buffer, s_font, GTextAlignmentRight, FTextAnchorBaseline);
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);

  // Date: weekday / month / day at top-left using system font
  if (s_show_date) {
    int16_t padding = PBL_IF_ROUND_ELSE(bounds.size.w / 10, 6);
    int16_t line_h = bounds.size.h / 11;
    int16_t start_y = PBL_IF_ROUND_ELSE(bounds.size.h / 6, line_h / 3);
    GFont date_font = fonts_get_system_font(bounds.size.h >= 200 ? FONT_KEY_GOTHIC_24_BOLD : FONT_KEY_GOTHIC_18_BOLD);
    GRect row = GRect(padding, start_y, bounds.size.w - padding, line_h + 4);
    graphics_context_set_text_color(ctx, s_text_color);
    graphics_draw_text(ctx, s_weekday_buffer, date_font, row, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
    row.origin.y += line_h;
    graphics_draw_text(ctx, s_month_buffer, date_font, row, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
    row.origin.y += line_h;
    graphics_draw_text(ctx, s_day_buffer, date_font, row, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  }

  // Seconds: mid-left using system font
  if (s_show_seconds) {
    int16_t padding = PBL_IF_ROUND_ELSE(bounds.size.w / 10, 6);
    GFont sec_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    GRect row = GRect(padding, bounds.size.h / 2, 60, 28);
    graphics_context_set_text_color(ctx, s_text_color);
    graphics_draw_text(ctx, s_seconds_buffer, sec_font, row, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  }
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  strftime(s_hour_buffer, sizeof(s_hour_buffer), clock_is_24h_style() ? "%H" : "%I", tick_time);
  strftime(s_minute_buffer, sizeof(s_minute_buffer), ":%M", tick_time);

  if (s_show_seconds) {
    strftime(s_seconds_buffer, sizeof(s_seconds_buffer), "%S", tick_time);
  }

  if (s_show_date) {
    strftime(s_weekday_buffer, sizeof(s_weekday_buffer), "%a", tick_time);
    strftime(s_month_buffer, sizeof(s_month_buffer), "%b", tick_time);
    strftime(s_day_buffer, sizeof(s_day_buffer), "%d", tick_time);
  }

  layer_mark_dirty(s_main_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void update_tick_timer() {
  tick_timer_service_unsubscribe();
  if (s_show_seconds) {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }
}

static void update_colors() {
  window_set_background_color(s_window, s_background_color);
  layer_mark_dirty(s_main_layer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *show_date_t = dict_find(iter, MESSAGE_KEY_ShowDate);
  if (show_date_t) {
    s_show_date = show_date_t->value->int32 == 1;
    persist_write_bool(MESSAGE_KEY_ShowDate, s_show_date);
    update_time();
  }

  Tuple *show_seconds_t = dict_find(iter, MESSAGE_KEY_ShowSeconds);
  if (show_seconds_t) {
    s_show_seconds = show_seconds_t->value->int32 == 1;
    persist_write_bool(MESSAGE_KEY_ShowSeconds, s_show_seconds);
    update_tick_timer();
    update_time();
  }

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
  GRect bounds = layer_get_unobstructed_bounds(window_layer);

  window_set_background_color(window, s_background_color);

  s_font = ffont_create_from_resource(RESOURCE_ID_AVENIR_FFONT);

  s_main_layer = layer_create(bounds);
  layer_set_update_proc(s_main_layer, main_layer_draw);
  layer_add_child(window_layer, s_main_layer);

  update_time();
}

static void prv_window_unload(Window *window) {
  layer_destroy(s_main_layer);
  ffont_destroy(s_font);
}

static void prv_init(void) {
  if (persist_exists(MESSAGE_KEY_ShowDate)) {
    s_show_date = persist_read_bool(MESSAGE_KEY_ShowDate);
  }
  if (persist_exists(MESSAGE_KEY_ShowSeconds)) {
    s_show_seconds = persist_read_bool(MESSAGE_KEY_ShowSeconds);
  }
  if (persist_exists(MESSAGE_KEY_TextColor)) {
    s_text_color = color_from_hex(persist_read_int(MESSAGE_KEY_TextColor));
  } else {
    s_text_color = GColorWhite;
  }
  if (persist_exists(MESSAGE_KEY_BackgroundColor)) {
    s_background_color = color_from_hex(persist_read_int(MESSAGE_KEY_BackgroundColor));
  } else {
    s_background_color = GColorBlack;
  }

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, true);

  update_tick_timer();
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
