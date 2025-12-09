#ifndef PLATFORM_NUCLEOF411RE_ILI9486_H
#define PLATFORM_NUCLEOF411RE_ILI9486_H

#include <stdint.h>

void platform_nucleof411re_ili9486_init(void);
void platform_nucleof411re_ili9486_selftest(void);
void platform_nucleof411re_ili9486_test_fill(uint16_t color);
void platform_nucleof411re_ili9486_test_colors_cycle(void);
void draw_rectangle (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void draw_triangle(uint16_t x, uint16_t y, uint16_t size, uint16_t color);



#endif