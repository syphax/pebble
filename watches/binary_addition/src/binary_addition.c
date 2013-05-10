
/*
  
  Another binary watch
  Adapted from Just A Bit source code 
  and along the lines of Binary Boxes http://www.mypebblefaces.com/view?fID=470&aName=docmarionum1&pageTitle=Binary+Boxes&auID=492
  Like A Bit Different, but I've replaced the circles with numbers
  This makes actually figuring out the time a bit easier
  
  B. Crounse
  
*/

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0xB1, 0x74, 0x72, 0xFA, 0x04, 0x44, 0x46, 0x88, 0x80, 0xE7, 0x40, 0xED, 0x64, 0x81, 0x72, 0xA0 }

PBL_APP_INFO(MY_UUID,
             "Binary Addition", "B. Crounse",
             0, 2, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

Layer display_layer;

#define CIRCLE_RADIUS 8
#define CIRCLE_LINE_THICKNESS 1

#define CELLS_PER_ROW 6
#define CELLS_PER_COLUMN 4

#define SCREEN_WIDTH 144
#define CELL_SIZE 20
#define CIRCLE_PADDING CELL_SIZE/2 - CIRCLE_RADIUS // Number of padding pixels on each side
#define SIDE_PADDING (SCREEN_WIDTH - (CELL_SIZE * CELLS_PER_ROW)) / 2
#define TOP_PADDING 0


void draw_cell(GContext* ctx, GPoint center, bool filled) {
  // Each "cell" represents a binary digit or 0 or 1.

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, CIRCLE_RADIUS);

  if (!filled) {
    // This is our ghetto way of drawing circles with a line thickness
    // of more than a single pixel.
    graphics_context_set_fill_color(ctx, GColorBlack);

    graphics_fill_circle(ctx, center, CIRCLE_RADIUS - CIRCLE_LINE_THICKNESS);
  }

}

void draw_number(GContext* ctx, int x, int y, bool show, const char* txt)  {
  
    if(show) {
      graphics_text_draw(ctx, txt, fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect(x, y, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);
    }
    
}

GPoint get_center_point_from_cell_location(unsigned short x, unsigned short y) {
  // Cell location (0,0) is upper left, location (4, 6) is lower right.
  return GPoint( SCREEN_WIDTH-(SIDE_PADDING + (CELL_SIZE/2) + (CELL_SIZE * x)),
		(CELL_SIZE/2) + (CELL_SIZE * y));
}

void draw_cell_row_for_digit(GContext* ctx, unsigned short digit, unsigned short max_columns_to_display, unsigned short cell_row) {
  
  const char *nums[8];
  nums[0] = " 1";
  nums[1] = " 2";
  nums[2] = " 4";
  nums[3] = " 8";
  nums[4] = "16";
  nums[5] = "32";
  nums[6] = "64";      
  nums[7] = "128";
  int x;
  int y;
  
  // Converts the supplied decimal digit into Binary Coded Decimal form and
  // then draws a row of cells on screen--'1' binary values are filled, '0' binary values are not filled.
  // `max_columns_to_display` restricts how many binary digits are shown in the row.
  
  for (int cell_column_index = 0; cell_column_index < max_columns_to_display; cell_column_index++) {
 
 //   draw_cell(ctx, get_center_point_from_cell_location(cell_column_index, cell_row), (digit >> cell_column_index) & 0x1);
    
    // TODO: Replace x,y with a GPoint
    x = SCREEN_WIDTH - (SIDE_PADDING + (CELL_SIZE * cell_column_index)) - CELL_SIZE;
    y = CELL_SIZE * cell_row;
    
    draw_number(ctx, x,y , (digit >> cell_column_index) & 0x1,nums[cell_column_index]);
  }
}

// The cell row offsets for each digit
#define HOURS_ROW 2
#define MINUTES_ROW 3
#define SECONDS_ROW 4
#define TEXT_ROW 5

// The maximum number of cell columns to display
#define DEFAULT_MAX_COLS 6
#define HOURS_MAX_COLS 5
#define MINUTES_MAX_COLS 6
#define SECONDS_MAX_COLS 6


unsigned short get_display_hour(unsigned short hour) {

  if (clock_is_24h_style()) {
    return hour;
  }

  unsigned short display_hour = hour % 12;

  // Converts "0" to "12"
  return display_hour ? display_hour : 12;

}


void display_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  PblTm t;

  get_time(&t);

  unsigned short display_hour = get_display_hour(t.tm_hour);

  draw_cell_row_for_digit(ctx, display_hour, HOURS_MAX_COLS, HOURS_ROW);
  draw_cell_row_for_digit(ctx, t.tm_min, MINUTES_MAX_COLS, MINUTES_ROW);
  draw_cell_row_for_digit(ctx, t.tm_sec, SECONDS_MAX_COLS, SECONDS_ROW);

// Yeah, there's a better way to do this

/*
  graphics_text_draw(ctx, "1", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect(SECONDS_MAX_COLS*CELL_SIZE, TEXT_ROW*CELL_SIZE, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);
  graphics_text_draw(ctx, "2", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect((SECONDS_MAX_COLS-1)*CELL_SIZE, TEXT_ROW*CELL_SIZE, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);
  graphics_text_draw(ctx, "4", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect((SECONDS_MAX_COLS-2)*CELL_SIZE, TEXT_ROW*CELL_SIZE, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);
  graphics_text_draw(ctx, "8", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect((SECONDS_MAX_COLS-3)*CELL_SIZE, TEXT_ROW*CELL_SIZE, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);
  graphics_text_draw(ctx, "16", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect((SECONDS_MAX_COLS-4)*CELL_SIZE-4, TEXT_ROW*CELL_SIZE, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);
  graphics_text_draw(ctx, "32", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect((SECONDS_MAX_COLS-5)*CELL_SIZE-4, TEXT_ROW*CELL_SIZE, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);
*/

  graphics_text_draw(ctx, "H", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect(0, HOURS_ROW*CELL_SIZE, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);
  graphics_text_draw(ctx, "M", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect(0, MINUTES_ROW*CELL_SIZE, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);
  graphics_text_draw(ctx, "S", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect(0, SECONDS_ROW*CELL_SIZE, CELL_SIZE, CELL_SIZE), GTextOverflowModeWordWrap, GAlignLeft,NULL);

}

void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx; // TODO: Pass tick event/PblTime rather than make layer use `get_time()`?

  layer_mark_dirty(&display_layer);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "A Bit Different Watch");
  window_stack_push(&window, true);

  window_set_background_color(&window, GColorBlack);

  // Init the layer for the display
  layer_init(&display_layer, window.layer.frame);
  display_layer.update_proc = &display_layer_update_callback;
  layer_add_child(&window.layer, &display_layer);


}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}