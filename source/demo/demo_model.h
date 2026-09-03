#ifndef DEMO_MODEL_H
#define DEMO_MODEL_H

#include <stdbool.h>
#include <stdint.h>

#include "game_engine/actor.h"

#define DEMO_SPEED_MIN 1U
#define DEMO_SPEED_MAX 6U

typedef struct {
    bool speed_up_pressed;
    bool speed_down_pressed;
    bool toggle_pause_pressed;
} demo_input_t;

typedef struct {
    actor_t actor;
    int16_t playfield_width;
    int16_t playfield_height;
    uint16_t bounce_count;
    uint8_t speed;
    bool paused;
} demo_model_t;

void demo_model_init(demo_model_t *model,
                     int16_t playfield_width,
                     int16_t playfield_height,
                     uint32_t now_ms);
bool demo_model_update(demo_model_t *model,
                       const demo_input_t *input,
                       uint32_t now_ms);

#endif /* DEMO_MODEL_H */
