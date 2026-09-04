#ifndef DEMO_VIEW_H
#define DEMO_VIEW_H

#include <stdbool.h>
#include <stdint.h>

#include "demo_model.h"
#include "graphics/stm32boy.h"

typedef struct {
    bool initialized;
    uint8_t level;
    uint8_t palette;
    bool heartbeat_on;
} demo_view_t;

void demo_view_init(demo_view_t *view);
void demo_view_render(stm32boy_t *graphics,
                      const demo_model_t *model,
                      demo_view_t *view);

#endif /* DEMO_VIEW_H */
