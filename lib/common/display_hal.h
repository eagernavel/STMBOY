#ifndef COMMON_DISPLAY_HAL_H
#define COMMON_DISPLAY_HAL_H

#include <stdint.h>

/* -----------------------------------------------------------------------
 * Hardware Abstraction Layer para el display.
 *
 * Define la interfaz mínima que el módulo gráfico necesita del hardware
 * de pantalla.  El módulo platform inyecta una implementación concreta
 * (ILI9486, ST7789, simulador SDL, …) sin que graphics conozca los
 * detalles.
 *
 * Patrón: Strategy / Dependency Injection via struct de punteros a
 * función — idiomático en C embebido.
 * ----------------------------------------------------------------------- */
typedef struct {
    /** Configura la ventana de escritura [x0,y0] – [x1,y1]. */
    void (*set_addr_window)(uint16_t x0, uint16_t y0,
                            uint16_t x1, uint16_t y1);

    /** Inicia una transacción de píxeles (CS low, modo data). */
    void (*begin_pixels)(void);

    /** Finaliza la transacción de píxeles (CS high). */
    void (*end_pixels)(void);

    /** Empuja `count` repeticiones del mismo color RGB565. */
    void (*push_color)(uint16_t color, uint32_t count);

    /** Empuja un buffer de `count` píxeles RGB565 consecutivos. */
    void (*push_pixels_rgb565)(const uint16_t *pixels, uint32_t count);
} display_hal_t;

#endif /* COMMON_DISPLAY_HAL_H */
