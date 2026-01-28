#ifndef PLATFORM_NUCLEOF411RE_ILI9486_H
#define PLATFORM_NUCLEOF411RE_ILI9486_H


#include <stdint.h>

void platform_nucleof411re_ili9486_init(void);
void platform_nucleof411re_ili9486_selftest(void);
void platform_nucleof411re_ili9486_test_fill(uint16_t color);
void platform_nucleof411re_ili9486_test_colors_cycle(void);
void draw_rectangle (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void draw_triangle(uint16_t x, uint16_t y, uint16_t size, uint16_t color);
void ili9486_display_init(void);
void ili9486_clear_screen(uint16_t color);
void ili9486_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ili9486_begin_pixels(void);
void ili9486_end_pixels(void);
void ili9486_push_color(uint16_t color, uint32_t count);
void ili9486_push_pixels_rgb565(const uint16_t *pixels, uint32_t count);


void draw_rectangle(uint16_t x, uint16_t y,
                    uint16_t width, uint16_t height,
                    uint16_t color);



#endif