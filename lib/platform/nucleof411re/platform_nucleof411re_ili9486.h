#ifndef PLATFORM_NUCLEOF411RE_ILI9486_H
#define PLATFORM_NUCLEOF411RE_ILI9486_H

#include <stdint.h>

void platform_nucleof411re_ili9486_init(void);
void platform_nucleof411re_ili9486_selftest(void);
void platform_nucleof411re_ili9486_test_fill(uint16_t color);
void platform_nucleof411re_ili9486_test_colors_cycle(void);



#endif
