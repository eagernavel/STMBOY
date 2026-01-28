#ifndef STM32BOY_H  
#define STM32BOY_H


#include <stdint.h>

typedef uint16_t stm32boy_color_t;

typedef struct {
    uint16_t width;
    uint16_t height;

    // Texto
    uint16_t cursor_x;
    uint16_t cursor_y;
    uint16_t text_fg;
    uint16_t text_bg;
    uint8_t  text_scale;        // 1..N
    uint8_t  text_transparent;  // 0/1
} stm32boy_t;

typedef enum {
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT
} text_align_h_t;

typedef enum {
    TEXT_ALIGN_TOP,
    TEXT_ALIGN_MIDDLE,
    TEXT_ALIGN_BOTTOM
} text_align_v_t;

typedef struct {
    uint16_t w;
    uint16_t h;
} text_size_t;

typedef struct {
    uint16_t x, y, w, h;
} rect_t;

void stm32_write_in_rect(stm32boy_t *g,
                         rect_t r,
                         text_align_h_t ah,
                         text_align_v_t av,
                         const char *s);



void stm32boy_init(stm32boy_t *g, uint16_t width, uint16_t height);

// Primitivas
void stm32_fillScreen(stm32boy_t*g, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void stm32_fillRect(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void stm32_drawRect(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void stm32boy_drawLine(stm32boy_t *g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

// Bitmaps / sprites
void stm32_drawBitmapRGB565(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h,
                            const uint16_t *pixels);

void stm32_drawSpriteRGB565_ck(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h,
                               const uint16_t *pixels, uint16_t colorkey);

// Texto (fuente 5x7)
void stm32_write_aligned(stm32boy_t *g,
                                uint16_t x, uint16_t y,
                                text_align_h_t ah,
                                text_align_v_t av,
                                const char *s);
text_size_t stm32_measure_text_wrap(stm32boy_t *g, const char *s);
void stm32_set_text_cursor(stm32boy_t *g, uint16_t x, uint16_t y);
void stm32_set_text_color(stm32boy_t *g, uint16_t fg, uint16_t bg, uint8_t transparent);
void stm32_set_text_scale(stm32boy_t *g, uint8_t scale);
void stm32_write(stm32boy_t *g, const char *s);
void stm32_drawChar(stm32boy_t *g, char c);

void stm32boy_drawPixel(stm32boy_t *g, int16_t x, int16_t y, stm32boy_color_t color);

void stm32boy_drawFastHLine(stm32boy_t *g, int16_t x, int16_t y, int16_t w, stm32boy_color_t color);
void stm32boy_drawFastVLine(stm32boy_t *g, int16_t x, int16_t y, int16_t h, stm32boy_color_t color);


#endif