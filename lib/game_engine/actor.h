#ifndef GAME_ENGINE_ACTOR_H
#define GAME_ENGINE_ACTOR_H

#include <stdint.h>
#include "game_engine/animation.h"

/* -----------------------------------------------------------------------
 * Actor: entidad con posición, velocidad y animación de sprite.
 * ----------------------------------------------------------------------- */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t prev_x;
    int16_t prev_y;
    int16_t w;
    int16_t h;
    int16_t vx;
    int16_t vy;
    uint32_t last_move_ms;
    uint32_t move_interval_ms;
    sprite_anim_t anim;
} actor_t;

/*
 * Inicializa un actor con posición, velocidad y animación.
 */
void actor_init(actor_t *a,
                int16_t x,
                int16_t y,
                int16_t w,
                int16_t h,
                int16_t vx,
                int16_t vy,
                uint32_t move_interval_ms,
                const sprite_t *frames,
                uint16_t frame_count,
                uint32_t frame_time_ms,
                uint32_t now_ms);

/*
 * Actualiza posición y animación del actor según el tiempo actual.
 *
 * Devuelve:
 *   1 -> si hubo algún cambio (movimiento o frame de animación)
 *   0 -> si no cambió nada
 */
uint8_t actor_update(actor_t *a, uint32_t now_ms);

/*
 * Rebota el actor dentro de los límites de pantalla.
 */
void actor_bounce_on_limits(actor_t *a, int16_t screen_w, int16_t screen_h);

#endif
