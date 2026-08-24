#ifndef BSP_H
#define BSP_H

#include <stdbool.h>
#include <stdint.h>

#include "common/display_hal.h"
#include "common/input.h"

bool bsp_init(void);
const display_hal_t *bsp_display(void);
void bsp_input_update(void);
const input_state_t *bsp_input_get(void);
uint32_t bsp_millis(void);

#endif
