#include <stdint.h>
#include <stdio.h>

#include "bsp/bsp.h"
#include "platform/nucleof411re/platform_nucleof411re_ili9486.h"



static inline void raw_delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
}

int main(void) {

    draw_rectangle (50, 50, 120, 150, 0xF800); // Draw red square
    
    
    while (1) {
    
        raw_delay(1000000);
    }

    return 0;
}
