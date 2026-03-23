#ifndef STM32BOY_H  
#define STM32BOY_H

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_GREEN 0x07E0


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
    uint8_t  text_transparent;
    uint16_t clear_color;   // 0/1
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


typedef struct {
    uint16_t w;
    uint16_t h;
    const uint16_t *pixels; // w*h RGB565
} sprite_t;

/*
 * Estructura de estado de una animacion de sprite.
 *
 * frames:
 *   Array de sprites que forman la animacion.
 *
 * frame_count:
 *   Numero total de frames.
 *
 * current_frame:
 *   Indice del frame actual.
 *
 * frame_duration_ms:
 *   Duracion de cada frame en milisegundos.
 *
 * last_tick_ms:
 *   Marca temporal del ultimo cambio de frame.
 */
typedef struct {
    const sprite_t *frames;
    uint16_t frame_count;
    uint16_t current_frame;
    uint32_t frame_duration_ms;
    uint32_t last_tick_ms;
} sprite_anim_t;

/*
 * Inicializa una animacion.
 *
 * anim              -> estructura a inicializar
 * frames            -> array de frames
 * frame_count       -> numero de frames del array
 * frame_duration_ms -> duracion de cada frame
 * now_ms            -> tiempo actual en ms
 */
void sprite_anim_init(sprite_anim_t *anim,
                      const sprite_t *frames,
                      uint16_t frame_count,
                      uint32_t frame_duration_ms,
                      uint32_t now_ms);

/*
 * Reinicia la animacion al frame 0.
 */
void sprite_anim_reset(sprite_anim_t *anim, uint32_t now_ms);

/*
 * Devuelve puntero al frame actual.
 * Si la animacion no es valida, devuelve 0.
 */
const sprite_t *sprite_anim_get_frame(const sprite_anim_t *anim);

/*
 * Actualiza la animacion segun el tiempo actual.
 *
 * Devuelve:
 *   1 -> si el frame ha cambiado
 *   0 -> si sigue en el mismo frame
 */
uint8_t sprite_anim_update(sprite_anim_t *anim, uint32_t now_ms);

/*
 * Dibuja el frame actual en la posicion indicada.
 */
void sprite_anim_draw(stm32boy_t *g,
                      int16_t x,
                      int16_t y,
                      const sprite_anim_t *anim);

void stm32boy_init(stm32boy_t *g, uint16_t width, uint16_t height);

// Primitivas
void stm32_fillScreen(stm32boy_t*g, uint16_t color);
void stm32_fillRect(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void stm32_drawRect(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void stm32_Line (stm32boy_t *g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void stm32_triangle(stm32boy_t *g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void stm32_polygon(stm32boy_t *g, const int16_t *points, uint16_t num_points, uint16_t color);
// Bitmaps / sprites
void stm32_drawBitmapRGB565(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h,
                            const uint16_t *pixels);
// Texto (fuente 5x7)
text_size_t stm32_measure_text_wrap(stm32boy_t *g, const char *s);
void stm32_set_text_cursor(stm32boy_t *g, uint16_t x, uint16_t y);
void stm32_set_text_color(stm32boy_t *g, uint16_t fg, uint16_t bg, uint8_t transparent);
void stm32_set_text_scale(stm32boy_t *g, uint8_t scale);
void stm32_write(stm32boy_t *g, const char *s);
void stm32_drawChar(stm32boy_t *g, char c);
void stm32_write_at(stm32boy_t *g, uint16_t x, uint16_t y, const char *s);                                 

void stm32boy_drawPixel(stm32boy_t *g, int16_t x, int16_t y, stm32boy_color_t color);

void stm32boy_drawFastHLine(stm32boy_t *g, int16_t x, int16_t y, int16_t w, stm32boy_color_t color);
void stm32boy_drawFastVLine(stm32boy_t *g, int16_t x, int16_t y, int16_t h, stm32boy_color_t color);

//sprites
void stm32_sprite(stm32boy_t *g, int16_t x, int16_t y, const sprite_t *sprite);


static const uint16_t sprite_alien_16x16[16*16] = {

COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,

COLOR_BLACK,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_BLACK,

COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,

COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_WHITE,COLOR_WHITE,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_WHITE,COLOR_WHITE,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,

COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_WHITE,COLOR_WHITE,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_WHITE,COLOR_WHITE,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,

COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,

COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,

COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,

COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,

COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,

COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,

COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,

COLOR_BLACK,COLOR_GREEN,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_GREEN,COLOR_BLACK,

COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,

COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,

COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_GREEN,COLOR_GREEN,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,

};


#endif