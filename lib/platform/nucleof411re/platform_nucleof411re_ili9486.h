#ifndef PLATFORM_NUCLEOF411RE_ILI9486_H
#define PLATFORM_NUCLEOF411RE_ILI9486_H

#include <stdbool.h>

#include "common/display_hal.h"

bool platform_nucleof411re_ili9486_init(void);
const display_hal_t *platform_nucleof411re_ili9486_display(void);

#endif /* PLATFORM_NUCLEOF411RE_ILI9486_H */
