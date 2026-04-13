#ifndef STM32BOY_H  
#define STM32BOY_H

#include <stdint.h>
#include "common/types.h"       /* sprite_t, rect_t, colores RGB565, stm32boy_color_t */
#include "common/display_hal.h" /* display_hal_t — interfaz abstracta del display */

/* -----------------------------------------------------------------------
 * Contexto gráfico principal.
 * El nombre "struct stm32boy" permite forward declarations desde otros
 * módulos (e.g. game_engine/actor.h) sin incluir este header completo.
 * ----------------------------------------------------------------------- */
typedef struct stm32boy {
    uint16_t width;
    uint16_t height;

    /* HAL del display — inyección de dependencia */
    const display_hal_t *display;

    // Texto
    uint16_t cursor_x;
    uint16_t cursor_y;
    uint16_t text_fg;
    uint16_t text_bg;
    uint8_t  text_scale;        // 1..N
    uint8_t  text_transparent;
    uint16_t clear_color;
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

/* -----------------------------------------------------------------------
 * Inicialización
 * ----------------------------------------------------------------------- */
void stm32boy_init(stm32boy_t *g, uint16_t width, uint16_t height,
                   const display_hal_t *display);

/* -----------------------------------------------------------------------
 * Primitivas 2D
 * ----------------------------------------------------------------------- */
void stm32boy_fill_screen(stm32boy_t *g, uint16_t color);
void stm32boy_fill_rect(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void stm32boy_draw_rect(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void stm32boy_draw_line(stm32boy_t *g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void stm32boy_draw_triangle(stm32boy_t *g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void stm32boy_draw_polygon(stm32boy_t *g, const int16_t *points, uint16_t num_points, uint16_t color);

void stm32boy_draw_pixel(stm32boy_t *g, int16_t x, int16_t y, stm32boy_color_t color);
void stm32boy_draw_fast_hline(stm32boy_t *g, int16_t x, int16_t y, int16_t w, stm32boy_color_t color);
void stm32boy_draw_fast_vline(stm32boy_t *g, int16_t x, int16_t y, int16_t h, stm32boy_color_t color);

/* -----------------------------------------------------------------------
 * Bitmaps / Sprites
 * ----------------------------------------------------------------------- */
void stm32boy_draw_bitmap_rgb565(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h,
                                 const uint16_t *pixels);
void stm32boy_draw_sprite(stm32boy_t *g, int16_t x, int16_t y, const sprite_t *sprite);

/* -----------------------------------------------------------------------
 * Texto (fuente 5x7)
 * ----------------------------------------------------------------------- */
text_size_t stm32boy_measure_text_wrap(stm32boy_t *g, const char *s);
void stm32boy_set_text_cursor(stm32boy_t *g, uint16_t x, uint16_t y);
void stm32boy_set_text_color(stm32boy_t *g, uint16_t fg, uint16_t bg, uint8_t transparent);
void stm32boy_set_text_scale(stm32boy_t *g, uint8_t scale);
void stm32boy_write(stm32boy_t *g, const char *s);
void stm32boy_draw_char(stm32boy_t *g, char c);
void stm32boy_write_at(stm32boy_t *g, uint16_t x, uint16_t y, const char *s);

#endif