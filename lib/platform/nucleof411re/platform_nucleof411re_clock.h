#ifndef PLATFORM_NUCLEOF411RE_CLOCK_H
#define PLATFORM_NUCLEOF411RE_CLOCK_H

#include <stdint.h>

typedef struct {
    uint32_t div_m;
    uint32_t mult_n;
    uint32_t div_p;
} platform_nucleof411re_clock_Pll;

typedef struct {
    uint32_t pwr_vos_scale;
    uint32_t flash_wait_cycles;
    platform_nucleof411re_clock_Pll pll;
    uint32_t ahb_prescaler;
    uint32_t apb1_prescaler;
    uint32_t apb2_prescaler;
} platform_nucleof411re_clock_Config;

void platform_nucleof411re_clock_init(const platform_nucleof411re_clock_Config config); 

#endif
