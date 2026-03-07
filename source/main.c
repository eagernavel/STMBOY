#include <stdint.h>
#include <stdio.h>

#include "bsp/bsp.h"
#include "platform/nucleof411re/stm32boy.h"

// Colores
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F
#define SPRITE_KEY 0xFFFF // color clave para transparencia en sprites

#define ILI9486_WIDTH   320
#define ILI9486_HEIGHT  480

static inline void raw_delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
}


int main(void)
{
    
    bsp_init();
    

    stm32boy_t g;
    stm32boy_init(&g, ILI9486_WIDTH, ILI9486_HEIGHT);

    stm32_fillScreen(&g, COLOR_WHITE);
    stm32_set_text_cursor(&g, 10, 10);
    stm32_set_text_color(&g, COLOR_BLACK, COLOR_WHITE, 0);
    stm32_set_text_scale(&g, 1);

    stm32_sprite(&g, 50, 50, &(sprite_t){
        .w = 16,
        .h = 16,
        .pixels = sprite_alien_16x16,
    }
);
    /*stm32_drawBitmapRGB565(&g, 50, 50, 5, 5, (const uint16_t[]){
    COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED,
    COLOR_RED, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_RED,
    COLOR_RED, COLOR_GREEN, COLOR_BLUE,  COLOR_GREEN, COLOR_RED,
    COLOR_RED, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_RED,
    COLOR_RED, COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED,
    });
    
    static uint16_t big[100*100];
    for (int i = 0; i < 100*100; i++) big[i] = COLOR_RED;
        stm32_drawBitmapRGB565(&g, 50, 50, 100, 100, big);
    */
    while (1) { }
}

