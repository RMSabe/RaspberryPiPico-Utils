/*
Example/Test for st7920_ctrl.h resource using a 128x64 Graphic LCD Display.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "pico.h"
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "st7920_ctrl.h"

st7920_t glcd = {
	.db0 = 10,
	.db1 = 11,
	.db2 = 12,
	.db3 = 13,
	.db4 = 6,
	.db5 = 7,
	.db6 = 8,
	.db7 = 9,
	.rs = 4,
	.e = 5
};

void proc0(void) //Draw a sine wave
{
	uint8_t cx = 0;
	uint8_t cy = 0;
	float x = 0.0f;
	float y = 0.0f;

	for(x = 0.0f; x < 6.28f; x += 0.01f)
	{
		y = sinf(x);
		cx = roundf(20*x);
		cy = 32 - roundf(28*y);
		st7920_load_graphic_buffer(&glcd, cx, cy);
	}

	st7920_paint_graphic_buffer(&glcd);
	return;
}

void proc1(void) //Draw a smiley face (:
{
	uint8_t cx = 0;
	uint8_t cy = 0;
	float x = 0.0f;

	for(x = 0.0f; x <= 6.28f; x += 0.01f)
	{
		cx = 64 - roundf(28*cosf(x));
		cy = 32 - roundf(28*sinf(x));
		st7920_load_graphic_buffer(&glcd, cx, cy);
	}

	for(x = 3.4f; x <= 6.02f; x += 0.01f)
	{
		cx = 64 - roundf(18*cosf(x));
		cy = 34 - roundf(14*sinf(x));
		st7920_load_graphic_buffer(&glcd, cx, cy);
	}

	st7920_load_graphic_buffer(&glcd, 53, 22);
	st7920_load_graphic_buffer(&glcd, 53, 23);
	st7920_load_graphic_buffer(&glcd, 54, 22);
	st7920_load_graphic_buffer(&glcd, 54, 23);

	st7920_load_graphic_buffer(&glcd, 73, 22);
	st7920_load_graphic_buffer(&glcd, 73, 23);
	st7920_load_graphic_buffer(&glcd, 74, 22);
	st7920_load_graphic_buffer(&glcd, 74, 23);

	st7920_paint_graphic_buffer(&glcd);
	return;
}

void proc2(void) //Draw some ASCII text
{
	st7920_set_text_cursor(&glcd, 0, 0);
	st7920_print_text(&glcd, "This is line 0");
	st7920_set_text_cursor(&glcd, 0, 1);
	st7920_print_text(&glcd, "This is line 1");
	st7920_set_text_cursor(&glcd, 0, 2);
	st7920_print_text(&glcd, "This is line 2");
	st7920_set_text_cursor(&glcd, 0, 3);
	st7920_print_text(&glcd, "This is line 3");

	return;
}

void proc3(void) //Draw some 16BIT GB character text
{
	const uint16_t text[7] = {0xA5E9, 0xA5D5, 0xA5A1, 0xA5A8, 0xA5EB, 0xA4B5, 0xA4F3}; //This is my name written in Japanese kana characters.
	st7920_set_text_cursor(&glcd, 0, 0);
	st7920_print_spchar_text(&glcd, text, 7);

	return;
}

int main(void)
{
	st7920_init(&glcd);
	while(true)
	{
		proc0();
		sleep_ms(5000);
		st7920_clear_graphic(&glcd);
		proc1();
		sleep_ms(5000);
		st7920_clear_graphic(&glcd);
		proc2();
		sleep_ms(5000);
		st7920_clear_text(&glcd);
		proc3();
		sleep_ms(5000);
		st7920_clear_text(&glcd);
	}

	return 0;
}
