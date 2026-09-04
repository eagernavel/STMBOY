/*
 * Prueba manual de color y direccionamiento del ILI9486.
 *
 * Para ejecutarla se sustituyo temporalmente source/main.c por este codigo.
 * Debe usarse con los LED retirados y el shield correctamente colocado.
 */

#include <stdint.h>

#include "bsp/bsp.h"
#include "common/display_hal.h"

#define COLOR_RED_RGB565   0xF800U
#define COLOR_GREEN_RGB565 0x07E0U
#define COLOR_BLUE_RGB565  0x001FU
#define COLOR_BLACK_RGB565 0x0000U

#define SOLID_COLOR_TIME_MS 5000U
#define COLOR_BARS_TIME_MS  8000U
#define BLACK_GAP_TIME_MS   2000U

static void halt(void)
{
    while (1) {
    }
}

static void wait_ms(uint32_t milliseconds)
{
    const uint32_t start = bsp_millis();
    while ((uint32_t)(bsp_millis() - start) < milliseconds) {
    }
}

static void fill_rect(const display_hal_t *display,
                      uint16_t x0,
                      uint16_t y0,
                      uint16_t x1,
                      uint16_t y1,
                      uint16_t color)
{
    const uint32_t pixel_count =
        (uint32_t)(x1 - x0 + 1U) * (uint32_t)(y1 - y0 + 1U);

    display->set_addr_window(display->context, x0, y0, x1, y1);
    display->begin_pixels(display->context);
    display->push_color(display->context, color, pixel_count);
    display->end_pixels(display->context);
}

static void fill_screen(const display_hal_t *display, uint16_t color)
{
    fill_rect(display, 0U, 0U,
              (uint16_t)(display->width - 1U),
              (uint16_t)(display->height - 1U), color);
}

static void show_color_bars(const display_hal_t *display)
{
    const uint16_t first_limit = (uint16_t)(display->width / 3U);
    const uint16_t second_limit = (uint16_t)((display->width * 2U) / 3U);
    const uint16_t bottom = (uint16_t)(display->height - 1U);

    fill_rect(display, 0U, 0U,
              (uint16_t)(first_limit - 1U), bottom, COLOR_RED_RGB565);
    fill_rect(display, first_limit, 0U,
              (uint16_t)(second_limit - 1U), bottom, COLOR_GREEN_RGB565);
    fill_rect(display, second_limit, 0U,
              (uint16_t)(display->width - 1U), bottom, COLOR_BLUE_RGB565);
}

int main(void)
{
    if (!bsp_init()) {
        halt();
    }

    const display_hal_t *display = bsp_display();
    if (display == 0 || display->width == 0U || display->height == 0U) {
        halt();
    }

    while (1) {
        fill_screen(display, COLOR_BLACK_RGB565);
        wait_ms(BLACK_GAP_TIME_MS);

        fill_screen(display, COLOR_RED_RGB565);
        wait_ms(SOLID_COLOR_TIME_MS);
        fill_screen(display, COLOR_BLACK_RGB565);
        wait_ms(BLACK_GAP_TIME_MS);

        fill_screen(display, COLOR_GREEN_RGB565);
        wait_ms(SOLID_COLOR_TIME_MS);
        fill_screen(display, COLOR_BLACK_RGB565);
        wait_ms(BLACK_GAP_TIME_MS);

        fill_screen(display, COLOR_BLUE_RGB565);
        wait_ms(SOLID_COLOR_TIME_MS);
        fill_screen(display, COLOR_BLACK_RGB565);
        wait_ms(BLACK_GAP_TIME_MS);

        show_color_bars(display);
        wait_ms(COLOR_BARS_TIME_MS);
    }
}

