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

    ili9486_clear_screen(COLOR_BLACK);
    stm32_set_text_cursor(&g, 10, 10);
    stm32_set_text_color(&g, COLOR_WHITE, COLOR_BLACK, 0);
    stm32_set_text_scale(&g, 2);
    stm32_write_aligned(&g,
                        ILI9486_WIDTH / 2,
                        ILI9486_HEIGHT / 2 - 20,
                        TEXT_ALIGN_LEFT,
                        TEXT_ALIGN_TOP,
                        "STM32BOY \r\n");
    
    rect_t r = { 0, 0, 320, 480 }; 
    stm32_write_in_rect(&g, r, TEXT_ALIGN_CENTER, TEXT_ALIGN_MIDDLE, "HELLO WORLD!\r\nFROM STM32BOY");


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

