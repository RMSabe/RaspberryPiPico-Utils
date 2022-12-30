//This is a resource to control the ST7920 Graphic LCD Controller.
//Notice that this code is meant to be used for the standard 128x64 pixel mapping Graphic LCD Screens.
//Different pixel mapping screens might not work properly with this code.

#include "st7920_ctrl.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "pico.h"
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

void st7920_print_byte(st7920_t *lcd, uint8_t byte)
{
	gpio_put(lcd->db7, (byte & 0x80));
	gpio_put(lcd->db6, (byte & 0x40));
	gpio_put(lcd->db5, (byte & 0x20));
	gpio_put(lcd->db4, (byte & 0x10));
	gpio_put(lcd->db3, (byte & 0x08));
	gpio_put(lcd->db2, (byte & 0x04));
	gpio_put(lcd->db1, (byte & 0x02));
	gpio_put(lcd->db0, (byte & 0x01));

	return;
}

void st7920_send_byte(st7920_t *lcd, bool reg, uint8_t byte)
{
	gpio_put(lcd->e, false);
	gpio_put(lcd->rs, reg);
	st7920_print_byte(lcd, byte);
	sleep_ms(1);
	gpio_put(lcd->e, true);
	sleep_ms(1);
	gpio_put(lcd->e, false);
	sleep_ms(1);

	return;
}

void st7920_init(st7920_t *lcd)
{
	gpio_init(lcd->db0);
	gpio_init(lcd->db1);
	gpio_init(lcd->db2);
	gpio_init(lcd->db3);
	gpio_init(lcd->db4);
	gpio_init(lcd->db5);
	gpio_init(lcd->db6);
	gpio_init(lcd->db7);
	gpio_init(lcd->rs);
	gpio_init(lcd->e);
	gpio_set_dir(lcd->db0, GPIO_OUT);
	gpio_set_dir(lcd->db1, GPIO_OUT);
	gpio_set_dir(lcd->db2, GPIO_OUT);
	gpio_set_dir(lcd->db3, GPIO_OUT);
	gpio_set_dir(lcd->db4, GPIO_OUT);
	gpio_set_dir(lcd->db5, GPIO_OUT);
	gpio_set_dir(lcd->db6, GPIO_OUT);
	gpio_set_dir(lcd->db7, GPIO_OUT);
	gpio_set_dir(lcd->rs, GPIO_OUT);
	gpio_set_dir(lcd->e, GPIO_OUT);

	uint8_t n = 0;
	while(n < 32)
	{
		memset(lcd->graphic_buffer[n], 0, 16*sizeof(uint16_t));
		n++;
	}

	st7920_send_byte(lcd, false, 0x30);
	st7920_send_byte(lcd, false, 0x01);
	st7920_send_byte(lcd, false, 0x02);

	return;
}

void st7920_set_display_mode(st7920_t *lcd, st7920_display_mode_t display_mode)
{
	st7920_send_byte(lcd, false, 0x30);
	switch(display_mode)
	{
		case ST7920_DISPLAY_OFF:
			st7920_send_byte(lcd, false, 0x08);
			break;

		case ST7920_CURSOR_OFF:
			st7920_send_byte(lcd, false, 0x0C);
			break;

		case ST7920_CURSOR_ON:
			st7920_send_byte(lcd, false, 0x0E);
			break;

		case ST7920_CURSOR_BLINK:
			st7920_send_byte(lcd, false, 0x0F);
			break;
	}

	return;
}

void st7920_load_graphic_buffer(st7920_t *lcd, uint8_t cx, uint8_t cy)
{
	uint8_t page = cx/16;
	cx %= 16;
	cx = 15 - cx;

	if(cy >= 32)
	{
		page += 8;
		cy -= 32;
	}

	lcd->graphic_buffer[cy][page] |= (1 << cx);
	return;
}

void st7920_paint_graphic_buffer(st7920_t *lcd)
{
	st7920_send_byte(lcd, false, 0x34);
	st7920_send_byte(lcd, false, 0x36);

	uint8_t cy = 0;
	uint8_t page = 0;
	uint16_t bit_frame = 0;
	uint8_t high_byte = 0;
	uint8_t low_byte = 0;

	for(cy = 0; cy < 32; cy++)
	{
		st7920_send_byte(lcd, false, (0x80 + cy));
		st7920_send_byte(lcd, false, 0x80);

		for(page = 0; page < 16; page++)
		{
			bit_frame = lcd->graphic_buffer[cy][page];
			high_byte = (bit_frame >> 8);
			low_byte = (bit_frame & 0x00FF);
			
			st7920_send_byte(lcd, true, high_byte);
			st7920_send_byte(lcd, true, low_byte);
		}
	}

	return;
}

void st7920_clear_graphic_buffer(st7920_t *lcd)
{
	uint8_t n = 0;
	while(n < 32)
	{
		memset(lcd->graphic_buffer[n], 0, 16*sizeof(uint16_t));
		n++;
	}

	return;
}

void st7920_clear_graphic(st7920_t *lcd)
{
	st7920_clear_graphic_buffer(lcd);
	st7920_paint_graphic_buffer(lcd);
	return;
}

void st7920_clear_text(st7920_t *lcd)
{
	st7920_send_byte(lcd, false, 0x30);
	st7920_send_byte(lcd, false, 0x01);
	return;
}

void st7920_set_text_cursor(st7920_t *lcd, uint8_t cx, uint8_t cy)
{
	st7920_send_byte(lcd, false, 0x30);

	if(cy%2) st7920_send_byte(lcd, false, 0x90);
	else st7920_send_byte(lcd, false, 0x80);

	uint8_t nshift = 0;

	if(cy > 1)
	{
		nshift = 0;
		while(nshift < 8)
		{
			st7920_send_byte(lcd, false, 0x14);
			nshift++;
		}
	}

	if(cx > 0)
	{
		nshift = 0;
		while(nshift < cx)
		{
			st7920_send_byte(lcd, false, 0x14);
			nshift++;
		}
	}

	return;
}

void st7920_print_text(st7920_t *lcd, const char *text)
{
	st7920_send_byte(lcd, false, 0x30);

	uint32_t nlen = 0;
	while(text[nlen] != '\0') nlen++;

	uint32_t nchar = 0;
	while(nchar < nlen)
	{
		st7920_send_byte(lcd, true, text[nchar]);
		nchar++;
	}

	return;
}

void st7920_print_spchar(st7920_t *lcd, const uint16_t spchar)
{
	uint8_t high_byte = (spchar >> 8);
	uint8_t low_byte = (spchar & 0x00FF);

	st7920_send_byte(lcd, false, 0x30);
	st7920_send_byte(lcd, true, high_byte);
	st7920_send_byte(lcd, true, low_byte);

	return;
}

void st7920_print_spchar_text(st7920_t *lcd, const uint16_t *spchar_text, uint32_t length)
{
	uint32_t nspchar = 0;
	while(nspchar < length)
	{
		st7920_print_spchar(lcd, spchar_text[nspchar]);
		nspchar++;
	}

	return;
}

void st7920_clear_all(st7920_t *lcd)
{
	st7920_clear_text(lcd);
	st7920_clear_graphic(lcd);
	return;
}
