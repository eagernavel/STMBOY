#include "platform/component/ili9486/ili9486.h"

#include <assert.h>
#include <stddef.h>

typedef struct {
    bool reset_values[2];
    uint8_t reset_count;
    uint8_t commands[16];
    uint8_t command_count;
    uint8_t data[24];
    uint8_t data_count;
    uint32_t delays[8];
    uint8_t delay_count;
    uint32_t begin_count;
    uint32_t end_count;
    uint32_t color_count;
    uint32_t pixel_count;
} fake_bus_t;

static void set_reset(void *context, bool asserted)
{
    fake_bus_t *fake = context;
    fake->reset_values[fake->reset_count++] = asserted;
}

static void write_command(void *context, uint8_t command)
{
    fake_bus_t *fake = context;
    fake->commands[fake->command_count++] = command;
}

static void write_data(void *context, uint8_t data)
{
    fake_bus_t *fake = context;
    fake->data[fake->data_count++] = data;
}

static void begin_pixels(void *context)
{
    ((fake_bus_t *)context)->begin_count++;
}

static void end_pixels(void *context)
{
    ((fake_bus_t *)context)->end_count++;
}

static void push_color(void *context, uint16_t color, uint32_t count)
{
    (void)color;
    ((fake_bus_t *)context)->color_count += count;
}

static void push_pixels(void *context,
                        const uint16_t *pixels,
                        uint32_t count)
{
    (void)pixels;
    ((fake_bus_t *)context)->pixel_count += count;
}

static void delay_ms(void *context, uint32_t milliseconds)
{
    fake_bus_t *fake = context;
    fake->delays[fake->delay_count++] = milliseconds;
}

int main(void)
{
    fake_bus_t fake = {0};
    ili9486_t device = {0};
    const ili9486_bus_t bus = {
        .context = &fake,
        .set_reset = set_reset,
        .write_command = write_command,
        .write_data = write_data,
        .begin_pixels = begin_pixels,
        .end_pixels = end_pixels,
        .push_color = push_color,
        .push_pixels_rgb565 = push_pixels,
        .delay_ms = delay_ms,
    };
    const ili9486_config_t config = {
        .width = 320U,
        .height = 480U,
        .mirror_x = true,
        .mirror_y = false,
        .swap_xy = false,
        .bgr = true,
    };

    assert(ili9486_init(&device, &bus, &config));
    assert(fake.reset_count == 2U);
    assert(fake.reset_values[0] && !fake.reset_values[1]);
    assert(fake.command_count == 6U);
    assert(fake.commands[0] == 0x01U);
    assert(fake.commands[1] == 0x11U);
    assert(fake.commands[2] == 0x3AU);
    assert(fake.commands[3] == 0x36U);
    assert(fake.commands[4] == 0x34U);
    assert(fake.commands[5] == 0x29U);
    assert(fake.data_count == 2U);
    assert(fake.data[0] == 0x55U && fake.data[1] == 0x48U);

    const display_hal_t *display = ili9486_display(&device);
    assert(display && display->context == &device);
    assert(display->width == 320U && display->height == 480U);

    display->set_addr_window(display->context, 1U, 2U, 3U, 4U);
    assert(fake.commands[6] == 0x2AU);
    assert(fake.commands[7] == 0x2BU);
    assert(fake.data_count == 10U);
    assert(fake.data[2] == 0U && fake.data[3] == 1U);
    assert(fake.data[4] == 0U && fake.data[5] == 3U);
    assert(fake.data[6] == 0U && fake.data[7] == 2U);
    assert(fake.data[8] == 0U && fake.data[9] == 4U);

    display->begin_pixels(display->context);
    display->push_color(display->context, 0xFFFFU, 7U);
    display->end_pixels(display->context);
    assert(fake.commands[8] == 0x2CU);
    assert(fake.begin_count == 1U && fake.end_count == 1U);
    assert(fake.color_count == 7U);

    return 0;
}
