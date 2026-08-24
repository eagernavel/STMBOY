#include "demo/demo_model.h"

#include <assert.h>

int main(void)
{
    demo_model_t model;
    demo_input_t input = {0};
    demo_model_init(&model, 0U);

    assert(model.level == 50U);
    assert(model.palette == 0U);
    assert(!model.heartbeat_on);

    input.up_held = true;
    assert(!demo_model_update(&model, &input, 34U));
    assert(demo_model_update(&model, &input, 35U));
    assert(model.level == 52U);

    input.up_held = false;
    input.next_palette_pressed = true;
    assert(demo_model_update(&model, &input, 40U));
    assert(model.palette == 1U);

    input.next_palette_pressed = false;
    assert(demo_model_update(&model, &input, 500U));
    assert(model.heartbeat_on);

    model.level = 1U;
    model.last_move_ms = 500U;
    input.down_held = true;
    assert(demo_model_update(&model, &input, 535U));
    assert(model.level == DEMO_LEVEL_MIN);

    input.up_held = true;
    assert(!demo_model_update(&model, &input, 570U));
    assert(model.level == DEMO_LEVEL_MIN);

    return 0;
}
