#include "bsp/bsp.h"
#include "demo/demo_model.h"
#include "demo/demo_view.h"
#include "graphics/stm32boy.h"

static void halt(void)
{
    while (1) {
    }
}

int main(void)
{
    if (!bsp_init()) {
        halt();
    }

    const display_hal_t *display = bsp_display();
    stm32boy_t graphics;
    if (!stm32boy_init(&graphics, display)) {
        halt();
    }

    demo_model_t model;
    demo_view_t view;
    demo_model_init(&model, bsp_millis());
    demo_view_init(&view);
    demo_view_render(&graphics, &model, &view);

    while (1) {
        bsp_input_update();
        const input_state_t *input = bsp_input_get();
        const demo_input_t demo_input = {
            .up_held = input->held[INPUT_UP],
            .down_held = input->held[INPUT_DOWN],
            .next_palette_pressed = input->pressed[INPUT_START],
        };

        if (demo_model_update(&model, &demo_input, bsp_millis())) {
            demo_view_render(&graphics, &model, &view);
        }
    }
}
