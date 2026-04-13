#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdint.h>

/* -----------------------------------------------------------------------
 * Colores RGB565 de uso frecuente.
 *
 * Centralizados aquí para que tanto graphics como game_engine (y
 * cualquier otro módulo) puedan utilizarlos sin crear dependencias
 * cruzadas.
 * ----------------------------------------------------------------------- */
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0

typedef uint16_t stm32boy_color_t;

/* -----------------------------------------------------------------------
 * Sprite: un frame estático (bitmap RGB565).
 *
 * Definido en common/ porque es un tipo de dato puro (sin lógica) que
 * necesitan tanto game_engine (animaciones/actores) como graphics
 * (dibujo de sprites).
 * ----------------------------------------------------------------------- */
typedef struct {
    uint16_t w;
    uint16_t h;
    const uint16_t *pixels; /* w*h píxeles RGB565 */
} sprite_t;

/* -----------------------------------------------------------------------
 * Rectángulo genérico.
 * ----------------------------------------------------------------------- */
typedef struct {
    uint16_t x, y, w, h;
} rect_t;

#endif /* COMMON_TYPES_H */
