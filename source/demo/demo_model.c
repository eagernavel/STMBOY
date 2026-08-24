#include "demo_model.h"

#define MOVE_INTERVAL_MS      35U
#define HEARTBEAT_INTERVAL_MS 500U
#define LEVEL_STEP            2U

void demo_model_init(demo_model_t *model, uint32_t now_ms)
{
    if (!model) return;

    model->level = 50U;
    model->palette = 0U;
    model->heartbeat_on = false;
    model->last_move_ms = now_ms;
    model->last_heartbeat_ms = now_ms;
}

bool demo_model_update(demo_model_t *model,
                       const demo_input_t *input,
                       uint32_t now_ms)
{
    if (!model || !input) return false;

    bool changed = false;

    if (input->next_palette_pressed) {
        model->palette = (uint8_t)((model->palette + 1U) %
                                   DEMO_PALETTE_COUNT);
        changed = true;
    }

    if ((now_ms - model->last_heartbeat_ms) >= HEARTBEAT_INTERVAL_MS) {
        model->last_heartbeat_ms = now_ms;
        model->heartbeat_on = !model->heartbeat_on;
        changed = true;
    }

    if (input->up_held != input->down_held &&
        (now_ms - model->last_move_ms) >= MOVE_INTERVAL_MS) {
        model->last_move_ms = now_ms;

        if (input->up_held && model->level < DEMO_LEVEL_MAX) {
            uint16_t next = (uint16_t)model->level + LEVEL_STEP;
            model->level = (uint8_t)(next > DEMO_LEVEL_MAX ?
                                     DEMO_LEVEL_MAX : next);
            changed = true;
        } else if (input->down_held && model->level > DEMO_LEVEL_MIN) {
            model->level = model->level < LEVEL_STEP ?
                           DEMO_LEVEL_MIN :
                           (uint8_t)(model->level - LEVEL_STEP);
            changed = true;
        }
    }

    return changed;
}
