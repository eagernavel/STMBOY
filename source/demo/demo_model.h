#ifndef DEMO_MODEL_H
#define DEMO_MODEL_H

#include <stdbool.h>
#include <stdint.h>

#define DEMO_LEVEL_MIN     0U
#define DEMO_LEVEL_MAX     100U
#define DEMO_PALETTE_COUNT 4U

typedef struct {
    bool up_held;
    bool down_held;
    bool next_palette_pressed;
} demo_input_t;

typedef struct {
    uint8_t level;
    uint8_t palette;
    bool heartbeat_on;
    uint32_t last_move_ms;
    uint32_t last_heartbeat_ms;
} demo_model_t;

void demo_model_init(demo_model_t *model, uint32_t now_ms);
bool demo_model_update(demo_model_t *model,
                       const demo_input_t *input,
                       uint32_t now_ms);

#endif /* DEMO_MODEL_H */
