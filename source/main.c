#include <stdint.h>
#include <stdio.h>

#include "bsp/bsp.h"
#include "platform/nucleof411re/platform_nucleof411re_ili9486.h"
#include "platform/nucleof411re/stm32boy.h"

// Colores
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F

#define ILI9486_WIDTH   320
#define ILI9486_HEIGHT  480

static inline void raw_delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
}

int main(void)
{
    
    platform_nucleof411re_ili9486_init();

    stm32boy_t g;
    stm32boy_init(&g, 320, 480);

    ili9486_clear_screen(COLOR_WHITE);
    stm32_set_text_cursor(&g, 10, 10);
    stm32_set_text_color(&g, COLOR_BLACK, COLOR_WHITE, 0);
    stm32_set_text_scale(&g, 1);
    stm32boy_drawPixel(&g, 300, 5, COLOR_BLUE);
    stm32boy_drawPixel(&g, 10, 20, COLOR_RED);

    rect_t r = { 0, 0, 320, 480}; 
    
    //stm32_write_aligned(&g, 160, 240, TEXT_ALIGN_BOTTOM, TEXT_ALIGN_MIDDLE, "STM32BOY");
    stm32_set_text_cursor(&g, 140, 240);
    stm32_write(&g, "STM32BOY");

                   


    //stm32_write(&g, "TEST 1 \r\n");
    //stm32_write(&g, "SEMPER FIDELIS \r\n");
    //stm32_write(&g, "DEUS VULT \r\n");
    //stm32_drawChar(&g, 'B');

    //stm32boy_drawLine(&gfx, 0, 0, 319, 479, COLOR_WHITE);
    //stm32boy_drawLine(&gfx, 319, 0, 0, 479, COLOR_WHITE);
    //stm32boy_drawFastHLine(&gfx, 0, 240, 320, COLOR_RED);
    //stm32boy_drawFastVLine(&gfx, 160, 0, 480, COLOR_RED);


    while (1) { }
}

