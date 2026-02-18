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
    
    //stm32_write_aligned(&g, 160, 240, TEXT_ALIGN_BOTTOM, TEXT_ALIGN_MIDDLE, "STM32BOY");
    stm32_set_text_cursor(&g, 140, 240);
    stm32_write(&g, "STM32BOY");
    stm32_Line(&g, 0, 0, 319, 479, COLOR_BLUE);
    stm32_Line(&g, 319, 0, 0, 479, COLOR_GREEN);
    stm32_Line(&g, 0, 240, 319, 240, COLOR_RED);
    stm32_Line(&g, 160, 0, 160, 479, COLOR_RED);
    stm32_drawRect(&g, 50, 50, 100, 150, COLOR_BLUE);
    stm32_fillRect(&g, 200, 50, 100, 150, COLOR_GREEN);
    stm32_triangle(&g, 50, 250, 150, 250, 100, 400, COLOR_RED);
    stm32_polygon(&g, (int16_t[]){200, 250, 300, 250, 350, 400, 250, 400, 150, 200}, 5, COLOR_BLUE);
                   


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

