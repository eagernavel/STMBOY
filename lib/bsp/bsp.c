#include "bsp.h"

#include "platform/platform.h"

bool bsp_init(void)
{
    return platform_init();
}

const display_hal_t *bsp_display(void)
{
    return platform_display();
}

void bsp_input_update(void)
{
    platform_input_update();
}

const input_state_t *bsp_input_get(void)
{
    return platform_input_get();
}

uint32_t bsp_millis(void)
{
    return platform_millis();
}
