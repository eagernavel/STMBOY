#include "demo/demo_model.h"
#include "demo/demo_view.h"
#include "graphics/stm32boy.h"

#include <stdio.h>

#define PREVIEW_WIDTH  320U
#define PREVIEW_HEIGHT 480U

typedef struct {
    uint16_t pixels[PREVIEW_WIDTH * PREVIEW_HEIGHT];
    uint16_t x0, y0, x1, y1;
    uint32_t cursor;
} preview_display_t;

static void set_window(void *context,
                       uint16_t x0, uint16_t y0,
                       uint16_t x1, uint16_t y1)
{
    preview_display_t *preview = context;
    preview->x0 = x0;
    preview->y0 = y0;
    preview->x1 = x1;
    preview->y1 = y1;
    preview->cursor = 0U;
}

static void begin_pixels(void *context)
{
    (void)context;
}

static void end_pixels(void *context)
{
    (void)context;
}

static void write_pixel(preview_display_t *preview, uint16_t color)
{
    uint16_t window_width = (uint16_t)(preview->x1 - preview->x0 + 1U);
    uint16_t x = (uint16_t)(preview->x0 + preview->cursor % window_width);
    uint16_t y = (uint16_t)(preview->y0 + preview->cursor / window_width);

    if (x <= preview->x1 && y <= preview->y1 &&
        x < PREVIEW_WIDTH && y < PREVIEW_HEIGHT) {
        preview->pixels[(uint32_t)y * PREVIEW_WIDTH + x] = color;
    }
    preview->cursor++;
}

static void push_color(void *context, uint16_t color, uint32_t count)
{
    preview_display_t *preview = context;
    while (count-- > 0U) {
        write_pixel(preview, color);
    }
}

static void push_pixels(void *context,
                        const uint16_t *pixels,
                        uint32_t count)
{
    preview_display_t *preview = context;
    while (count-- > 0U) {
        write_pixel(preview, *pixels++);
    }
}

static void write_u16_le(FILE *file, uint16_t value)
{
    fputc((int)(value & 0xFFU), file);
    fputc((int)(value >> 8), file);
}

static void write_u32_le(FILE *file, uint32_t value)
{
    write_u16_le(file, (uint16_t)(value & 0xFFFFU));
    write_u16_le(file, (uint16_t)(value >> 16));
}

static int write_bmp(const char *path, const preview_display_t *preview)
{
    const uint32_t row_size = PREVIEW_WIDTH * 3U;
    const uint32_t pixel_bytes = row_size * PREVIEW_HEIGHT;
    FILE *file = fopen(path, "wb");
    if (!file) return 1;

    fputc('B', file);
    fputc('M', file);
    write_u32_le(file, 54U + pixel_bytes);
    write_u32_le(file, 0U);
    write_u32_le(file, 54U);

    write_u32_le(file, 40U);
    write_u32_le(file, PREVIEW_WIDTH);
    write_u32_le(file, PREVIEW_HEIGHT);
    write_u16_le(file, 1U);
    write_u16_le(file, 24U);
    write_u32_le(file, 0U);
    write_u32_le(file, pixel_bytes);
    write_u32_le(file, 2835U);
    write_u32_le(file, 2835U);
    write_u32_le(file, 0U);
    write_u32_le(file, 0U);

    for (int32_t y = (int32_t)PREVIEW_HEIGHT - 1; y >= 0; --y) {
        for (uint32_t x = 0; x < PREVIEW_WIDTH; ++x) {
            uint16_t color = preview->pixels[(uint32_t)y * PREVIEW_WIDTH + x];
            uint8_t bgr[3] = {
                (uint8_t)(((color & 0x1FU) * 255U) / 31U),
                (uint8_t)((((color >> 5) & 0x3FU) * 255U) / 63U),
                (uint8_t)((((color >> 11) & 0x1FU) * 255U) / 31U),
            };
            fwrite(bgr, sizeof(bgr), 1U, file);
        }
    }

    fclose(file);
    return 0;
}

int main(int argc, char **argv)
{
    const char *output = argc > 1 ? argv[1] : "demo-preview.bmp";
    preview_display_t preview = {0};
    const display_hal_t display = {
        .context = &preview,
        .width = PREVIEW_WIDTH,
        .height = PREVIEW_HEIGHT,
        .set_addr_window = set_window,
        .begin_pixels = begin_pixels,
        .end_pixels = end_pixels,
        .push_color = push_color,
        .push_pixels_rgb565 = push_pixels,
    };

    stm32boy_t graphics;
    if (!stm32boy_init(&graphics, &display)) return 2;

    demo_model_t model;
    demo_view_t view;
    demo_model_init(&model, 0U);
    demo_view_init(&view);
    demo_view_render(&graphics, &model, &view);

    model.level = 72U;
    model.palette = 1U;
    model.heartbeat_on = true;
    demo_view_render(&graphics, &model, &view);

    return write_bmp(output, &preview);
}
