#include "graphics/stm32boy.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    uint32_t set_window_calls;
    uint32_t begin_calls;
    uint32_t end_calls;
    uint32_t push_color_calls;
    uint32_t push_pixels_calls;
    uint32_t pushed_pixels;
    uint16_t x0, y0, x1, y1;
    uint16_t color;
    const uint16_t *pixel_sources[8];
    bool saw_l_bottom_right;
    bool saw_l_top_right;
} fake_display_t;

static void fake_set_window(void *context,
                            uint16_t x0, uint16_t y0,
                            uint16_t x1, uint16_t y1)
{
    fake_display_t *fake = context;
    fake->set_window_calls++;
    fake->x0 = x0;
    fake->y0 = y0;
    fake->x1 = x1;
    fake->y1 = y1;
    if (x0 == 14U && y0 == 26U) fake->saw_l_bottom_right = true;
    if (x0 == 14U && y0 == 20U) fake->saw_l_top_right = true;
}

static void fake_begin(void *context)
{
    ((fake_display_t *)context)->begin_calls++;
}

static void fake_end(void *context)
{
    ((fake_display_t *)context)->end_calls++;
}

static void fake_push_color(void *context, uint16_t color, uint32_t count)
{
    fake_display_t *fake = context;
    fake->push_color_calls++;
    fake->color = color;
    fake->pushed_pixels += count;
}

static void fake_push_pixels(void *context,
                             const uint16_t *pixels,
                             uint32_t count)
{
    fake_display_t *fake = context;
    if (fake->push_pixels_calls < 8U) {
        fake->pixel_sources[fake->push_pixels_calls] = pixels;
    }
    fake->push_pixels_calls++;
    fake->pushed_pixels += count;
}

static display_hal_t make_display(fake_display_t *fake)
{
    return (display_hal_t) {
        .context = fake,
        .width = 100U,
        .height = 80U,
        .set_addr_window = fake_set_window,
        .begin_pixels = fake_begin,
        .end_pixels = fake_end,
        .push_color = fake_push_color,
        .push_pixels_rgb565 = fake_push_pixels,
    };
}

int main(void)
{
    fake_display_t fake = {0};
    display_hal_t display = make_display(&fake);
    stm32boy_t graphics;

    assert(stm32boy_init(&graphics, &display));
    assert(graphics.width == 100U);
    assert(graphics.height == 80U);

    stm32boy_fill_rect(&graphics, -5, -3, 10, 8, COLOR_RED);
    assert(fake.set_window_calls == 1U);
    assert(fake.x0 == 0U && fake.y0 == 0U);
    assert(fake.x1 == 4U && fake.y1 == 4U);
    assert(fake.color == COLOR_RED);
    assert(fake.pushed_pixels == 25U);

    memset(&fake, 0, sizeof(fake));
    uint16_t pixels[12] = {0};
    stm32boy_draw_bitmap_rgb565(&graphics, -1, 1, 4, 3, pixels);
    assert(fake.set_window_calls == 3U);
    assert(fake.push_pixels_calls == 3U);
    assert(fake.pushed_pixels == 9U);
    assert(fake.pixel_sources[0] == &pixels[1]);
    assert(fake.pixel_sources[1] == &pixels[5]);
    assert(fake.pixel_sources[2] == &pixels[9]);

    memset(&fake, 0, sizeof(fake));
    stm32boy_set_text_cursor(&graphics, 10U, 20U);
    stm32boy_draw_char(&graphics, 'L');
    assert(fake.saw_l_bottom_right);
    assert(!fake.saw_l_top_right);

    stm32boy_set_text_cursor(&graphics, 0U, 0U);
    stm32boy_write(&graphics, "A\nB");
    assert(graphics.cursor_x == 6U);
    assert(graphics.cursor_y == 8U);

    return 0;
}
