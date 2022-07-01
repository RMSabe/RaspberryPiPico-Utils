//Resource to control LCD Displays.

#include "lcd_ctrl.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "pico.h"
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

void lcd_print_bin(lcd_t *lcd, uint8_t bin)
{
  gpio_put(lcd->db7, (bin & 0x8));
  gpio_put(lcd->db6, (bin & 0x4));
  gpio_put(lcd->db5, (bin & 0x2));
  gpio_put(lcd->db4, (bin & 0x1));
  return;
}

void lcd_send_byte(lcd_t *lcd, bool reg, uint8_t byte)
{
  uint8_t high_bits = (byte >> 4);
  uint8_t low_bits = (byte & 0x0F);
  
  gpio_put(lcd->e, false);
  gpio_put(lcd->rs, reg);
  lcd_print_bin(lcd, high_bits);
  sleep_ms(1);
  gpio_put(lcd->e, true);
  sleep_ms(1);
  gpio_put(lcd->e, false);
  lcd_print_bin(lcd, low_bits);
  sleep_ms(1);
  gpio_put(lcd->e, true);
  sleep_ms(1);
  gpio_put(lcd->e, false);
  sleep_ms(1);
  
  return;
}

void lcd_send_init_bits(lcd_t *lcd)
{
  gpio_put(lcd->e, false);
  gpio_put(lcd->rs, false);
  lcd_print_bin(lcd, 0x2);
  sleep_ms(1);
  gpio_put(lcd->e, true);
  sleep_ms(1);
  gpio_put(lcd->e, false);
  sleep_ms(2);
  
  return;
}

void lcd_init(lcd_t *lcd)
{
  gpio_init(lcd->db4);
  gpio_init(lcd->db5);
  gpio_init(lcd->db6);
  gpio_init(lcd->db7);
  gpio_init(lcd->rs);
  gpio_init(lcd->e);
  
  gpio_set_dir(lcd->db4, GPIO_OUT);
  gpio_set_dir(lcd->db5, GPIO_OUT);
  gpio_set_dir(lcd->db6, GPIO_OUT);
  gpio_set_dir(lcd->db7, GPIO_OUT);
  gpio_set_dir(lcd->rs, GPIO_OUT);
  gpio_set_dir(lcd->e, GPIO_OUT);
  
  lcd_send_init_bits(lcd);
  lcd_send_byte(lcd, false, 0x28);
  lcd_send_byte(lcd, false, 0x01);
  lcd_send_byte(lcd, false, 0x0C);
  lcd_send_byte(lcd, false, 0x80);
  
  return;
}

void lcd_clear(lcd_t *lcd)
{
  lcd_send_byte(lcd, false, 0x01);
  return;
}

void lcd_home(lcd_t *lcd)
{
  lcd_send_byte(lcd, false, 0x02);
  return;
}

void lcd_set_display_mode(lcd_t *lcd, lcd_display_mode_t display_mode)
{
  switch(display_mode)
  {
    case LCD_DISPLAY_OFF:
      lcd_send_byte(lcd, false, 0x08);
      break;
      
    case LCD_CURSOR_OFF:
      lcd_send_byte(lcd, false, 0x0C);
      break;
      
    case LCD_CURSOR_ON:
      lcd_send_byte(lcd, false, 0x0E);
      break;
      
    case LCD_CURSOR_BLINK:
      lcd_send_byte(lcd, false, 0x0F);
      break;
  }
  
  return;
}

void lcd_set_cursor(lcd_t *lcd, uint8_t cx, uint8_t cy)
{
  if(cy >= lcd->n_lines) return;
  
  if(cy%2) lcd_send_byte(lcd, false, 0xC0);
  else lcd_send_byte(lcd, false, 0x80);
  
  uint8_t nline = 0;
  uint8_t nchar = 0;
  
  if(cy > 1)
  {
    for(nline = 0; nline < cy/2; nline++)
      for(nchar = 0; nchar < lcd->n_char_per_line; nchar++)
        lcd_send_byte(lcd, false, 0x14);
  }
  
  if(cx > 0)
  {
    nchar = 0;
    do{
      lcd_send_byte(lcd, false, 0x14);
      nchar++;
    }while(nchar < cx);
  }
  
  return;
}

void lcd_print_text(lcd_t *lcd, const char *text)
{
  uint32_t nlen = 0;
  while(text[nlen] != '\0') nlen++;
  
  uint32_t nchar = 0;
  while(nchar < nlen)
  {
    lcd_send_byte(lcd, true, text[nchar]);
    nchar++;
  }
  
  return;
}
