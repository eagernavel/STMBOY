#include "ili9486.h"

#define ILI9486_CMD_SOFTWARE_RESET     0x01U
#define ILI9486_CMD_SLEEP_OUT          0x11U
#define ILI9486_CMD_DISPLAY_ON         0x29U
#define ILI9486_CMD_COLUMN_ADDRESS_SET 0x2AU
#define ILI9486_CMD_PAGE_ADDRESS_SET   0x2BU
#define ILI9486_CMD_MEMORY_WRITE       0x2CU
#define ILI9486_CMD_MEMORY_ACCESS      0x36U
#define ILI9486_CMD_PIXEL_FORMAT       0x3AU
#define ILI9486_CMD_TEARING_OFF        0x34U

#define ILI9486_PIXEL_FORMAT_RGB565    0x55U
#define ILI9486_MADCTL_MY              0x80U
#define ILI9486_MADCTL_MX              0x40U
#define ILI9486_MADCTL_MV              0x20U
#define ILI9486_MADCTL_BGR             0x08U

static bool bus_is_valid(const ili9486_bus_t *bus)
{
    return bus && bus->set_reset && bus->write_command && bus->write_data &&
           bus->begin_pixels && bus->end_pixels && bus->push_color &&
           bus->push_pixels_rgb565 && bus->delay_ms;
}

static uint8_t build_madctl(const ili9486_config_t *config)
{
    uint8_t value = 0U;
    if (config->mirror_y) value |= ILI9486_MADCTL_MY;
    if (config->mirror_x) value |= ILI9486_MADCTL_MX;
    if (config->swap_xy) value |= ILI9486_MADCTL_MV;
    if (config->bgr) value |= ILI9486_MADCTL_BGR;
    return value;
}

static void write_u16(ili9486_t *device, uint16_t value)
{
    device->bus.write_data(device->bus.context, (uint8_t)(value >> 8));
    device->bus.write_data(device->bus.context, (uint8_t)(value & 0xFFU));
}

static void display_set_addr_window(void *context,
                                    uint16_t x0, uint16_t y0,
                                    uint16_t x1, uint16_t y1)
{
    ili9486_t *device = context;

    device->bus.write_command(device->bus.context,
                              ILI9486_CMD_COLUMN_ADDRESS_SET);
    write_u16(device, x0);
    write_u16(device, x1);

    device->bus.write_command(device->bus.context,
                              ILI9486_CMD_PAGE_ADDRESS_SET);
    write_u16(device, y0);
    write_u16(device, y1);
}

static void display_begin_pixels(void *context)
{
    ili9486_t *device = context;
    device->bus.write_command(device->bus.context, ILI9486_CMD_MEMORY_WRITE);
    device->bus.begin_pixels(device->bus.context);
}

static void display_end_pixels(void *context)
{
    ili9486_t *device = context;
    device->bus.end_pixels(device->bus.context);
}

static void display_push_color(void *context, uint16_t color, uint32_t count)
{
    ili9486_t *device = context;
    device->bus.push_color(device->bus.context, color, count);
}

static void display_push_pixels(void *context,
                                const uint16_t *pixels,
                                uint32_t count)
{
    ili9486_t *device = context;
    device->bus.push_pixels_rgb565(device->bus.context, pixels, count);
}

bool ili9486_init(ili9486_t *device,
                  const ili9486_bus_t *bus,
                  const ili9486_config_t *config)
{
    if (!device || !bus_is_valid(bus) || !config ||
        config->width == 0U || config->height == 0U) {
        return false;
    }

    device->bus = *bus;
    device->display = (display_hal_t) {
        .context = device,
        .width = config->width,
        .height = config->height,
        .set_addr_window = display_set_addr_window,
        .begin_pixels = display_begin_pixels,
        .end_pixels = display_end_pixels,
        .push_color = display_push_color,
        .push_pixels_rgb565 = display_push_pixels,
    };

    bus->set_reset(bus->context, true);
    bus->delay_ms(bus->context, 10U);
    bus->set_reset(bus->context, false);
    bus->delay_ms(bus->context, 120U);

    bus->write_command(bus->context, ILI9486_CMD_SOFTWARE_RESET);
    bus->delay_ms(bus->context, 5U);
    bus->write_command(bus->context, ILI9486_CMD_SLEEP_OUT);
    bus->delay_ms(bus->context, 120U);

    bus->write_command(bus->context, ILI9486_CMD_PIXEL_FORMAT);
    bus->write_data(bus->context, ILI9486_PIXEL_FORMAT_RGB565);
    bus->write_command(bus->context, ILI9486_CMD_MEMORY_ACCESS);
    bus->write_data(bus->context, build_madctl(config));
    bus->write_command(bus->context, ILI9486_CMD_TEARING_OFF);
    bus->write_command(bus->context, ILI9486_CMD_DISPLAY_ON);
    bus->delay_ms(bus->context, 20U);

    device->initialized = true;
    return true;
}

const display_hal_t *ili9486_display(const ili9486_t *device)
{
    if (!device || !device->initialized) {
        return 0;
    }
    return &device->display;
}
