#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

#include "common/display_hal.h"
#include "common/input.h"

bool platform_init(void);
const display_hal_t *platform_display(void);
void platform_input_update(void);
const input_state_t *platform_input_get(void);
uint32_t platform_millis(void);

#endif
