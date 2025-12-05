#include <stdint.h>
#include <stdio.h>

#include "bsp/bsp.h"
#include "platform/nucleof411re/platform_nucleof411re_ili9486.h"



static inline void raw_delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
}

int main(void) {


    bsp_init();

    platform_nucleof411re_ili9486_test_colors_cycle();// Fill screen with red color
    while (1) {
    
        raw_delay(1000000);
    }

    return 0;
}
