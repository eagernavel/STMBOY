#ifndef PLATFORM_COMPONENT_ILI9486_H
#define PLATFORM_COMPONENT_ILI9486_H

#include <stdbool.h>
#include <stdint.h>

#include "common/display_hal.h"

/* Operaciones físicas que debe proporcionar la plataforma. */
typedef struct {
    void *context;
    void (*set_reset)(void *context, bool asserted);
    void (*write_command)(void *context, uint8_t command);
    void (*write_data)(void *context, uint8_t data);
    void (*begin_pixels)(void *context);
    void (*end_pixels)(void *context);
    void (*push_color)(void *context, uint16_t color, uint32_t count);
    void (*push_pixels_rgb565)(void *context,
                               const uint16_t *pixels,
                               uint32_t count);
    void (*delay_ms)(void *context, uint32_t milliseconds);
} ili9486_bus_t;

typedef struct {
    ili9486_bus_t bus;
    display_hal_t display;
    bool initialized;
} ili9486_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    bool mirror_x;
    bool mirror_y;
    bool swap_xy;
    bool bgr;
} ili9486_config_t;

bool ili9486_init(ili9486_t *device,
                  const ili9486_bus_t *bus,
                  const ili9486_config_t *config);

const display_hal_t *ili9486_display(const ili9486_t *device);

#endif /* PLATFORM_COMPONENT_ILI9486_H */
