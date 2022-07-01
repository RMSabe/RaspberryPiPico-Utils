//Resource to control LCD displays.

#ifndef LCD_CTRL_H
#define LCD_CTRL_H

#include <stdint.h>

typedef enum {
  LCD_DISPLAY_OFF = 0,
  LCD_CURSOR_OFF,
  LCD_CURSOR_ON,
  LCD_CURSOR_BLINK
} lcd_display_mode_t;

typedef struct {
  uint8_t db4;
  uint8_t db5;
  uint8_t db6;
  uint8_t db7;
  uint8_t rs;
  uint8_t e;
  uint8_t n_char_per_line;
  uint8_t n_lines;
} lcd_t;

void lcd_init(lcd_t *lcd);
void lcd_clear(lcd_t *lcd);
void lcd_home(lcd_t *lcd);
void lcd_set_display_mode(lcd_t *lcd, lcd_display_mode_t display_mode);
void lcd_set_cursor(lcd_t *lcd, uint8_t cx, uint8_t cy);
void lcd_print_text(lcd_t *lcd, const char *text);

#endif
