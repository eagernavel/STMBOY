#ifndef COMMON_DISPLAY_HAL_H
#define COMMON_DISPLAY_HAL_H

#include <stdint.h>

/* Interfaz mínima que graphics necesita de cualquier display RGB565. */
typedef struct {
    void *context;
    uint16_t width;
    uint16_t height;

    void (*set_addr_window)(void *context,
                            uint16_t x0, uint16_t y0,
                            uint16_t x1, uint16_t y1);
    void (*begin_pixels)(void *context);
    void (*end_pixels)(void *context);
    void (*push_color)(void *context, uint16_t color, uint32_t count);
    void (*push_pixels_rgb565)(void *context,
                               const uint16_t *pixels,
                               uint32_t count);
} display_hal_t;

#endif /* COMMON_DISPLAY_HAL_H */
