#include <stdint.h>
#include <stdio.h>

#include "bsp/bsp.h"
#include "platform/nucleof411re/platform_nucleof411re_ili9486.h"



static inline void raw_delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
}

int main(void) {

    int x = 0;
    platform_nucleof411re_ili9486_init();

    for (int i = 0; i < 100; i++) {
     
        draw_rectangle (x, 50, 20, 150, 0xF800);
     
        x += i;
    }
    draw_rectangle (50, 50, 120, 150, 0xF800);
    draw_triangle(200,50,60,0xF800);
    platform_nucleof411re_ili9486_test_colors_cycle();// Draw red square
    
    
    while (1) {
    
        raw_delay(1000000);
    }

    return 0;
}
