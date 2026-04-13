#ifndef GAME_ENGINE_ANIMATION_H
#define GAME_ENGINE_ANIMATION_H

#include <stdint.h>
#include "common/types.h"   /* sprite_t, colores RGB565 */

/* -----------------------------------------------------------------------
 * Sprite Animation: secuencia de frames con temporización.
 *
 * frames            -> Array de sprites que forman la animación.
 * frame_count       -> Número total de frames.
 * current_frame     -> Índice del frame actual.
 * frame_duration_ms -> Duración de cada frame en milisegundos.
 * last_tick_ms      -> Marca temporal del último cambio de frame.
 * ----------------------------------------------------------------------- */
typedef struct {
    const sprite_t *frames;
    uint16_t frame_count;
    uint16_t current_frame;
    uint32_t frame_duration_ms;
    uint32_t last_tick_ms;
} sprite_anim_t;

/*
 * Inicializa una animación.
 *
 * anim              -> estructura a inicializar
 * frames            -> array de frames
 * frame_count       -> número de frames del array
 * frame_duration_ms -> duración de cada frame
 * now_ms            -> tiempo actual en ms
 */
void sprite_anim_init(sprite_anim_t *anim,
                      const sprite_t *frames,
                      uint16_t frame_count,
                      uint32_t frame_duration_ms,
                      uint32_t now_ms);

/*
 * Reinicia la animación al frame 0.
 */
void sprite_anim_reset(sprite_anim_t *anim, uint32_t now_ms);

/*
 * Devuelve puntero al frame actual.
 * Si la animación no es válida, devuelve 0.
 */
const sprite_t *sprite_anim_get_frame(const sprite_anim_t *anim);

/*
 * Actualiza la animación según el tiempo actual.
 *
 * Devuelve:
 *   1 -> si el frame ha cambiado
 *   0 -> si sigue en el mismo frame
 */
uint8_t sprite_anim_update(sprite_anim_t *anim, uint32_t now_ms);

#endif
