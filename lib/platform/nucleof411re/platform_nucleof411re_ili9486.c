#include "platform_nucleof411re_ili9486.h"

#include "platform/component/ili9486/ili9486.h"

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_utils.h"

#include <stddef.h>
#include <stdint.h>

#include "db_lut.h"

#define ILI9486_RES_GPIO GPIOC
#define ILI9486_RES_PIN  LL_GPIO_PIN_1

#define ILI9486_CS_GPIO GPIOB
#define ILI9486_CS_PIN  LL_GPIO_PIN_0

#define ILI9486_DCX_GPIO GPIOA
#define ILI9486_DCX_PIN  LL_GPIO_PIN_4

#define ILI9486_WR_GPIO GPIOA
#define ILI9486_WR_PIN  LL_GPIO_PIN_1

#define ILI9486_RD_GPIO GPIOA
#define ILI9486_RD_PIN  LL_GPIO_PIN_0

#define ILI9486_DB0_PIN LL_GPIO_PIN_9
#define ILI9486_DB1_PIN LL_GPIO_PIN_7
#define ILI9486_DB2_PIN LL_GPIO_PIN_10
#define ILI9486_DB3_PIN LL_GPIO_PIN_3
#define ILI9486_DB4_PIN LL_GPIO_PIN_5
#define ILI9486_DB5_PIN LL_GPIO_PIN_4
#define ILI9486_DB6_PIN LL_GPIO_PIN_10
#define ILI9486_DB7_PIN LL_GPIO_PIN_8

#define ILI9486_WIDTH  320U
#define ILI9486_HEIGHT 480U

static ili9486_t s_ili9486;

static void init_interface_pins(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull = LL_GPIO_PULL_NO;

    gpio.Pin = ILI9486_RES_PIN | ILI9486_DB1_PIN;
    LL_GPIO_Init(GPIOC, &gpio);

    gpio.Pin = ILI9486_WR_PIN | ILI9486_RD_PIN | ILI9486_DB0_PIN |
               ILI9486_DB2_PIN | ILI9486_DB7_PIN | ILI9486_DCX_PIN;
    LL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = ILI9486_CS_PIN | ILI9486_DB3_PIN | ILI9486_DB4_PIN |
               ILI9486_DB5_PIN | ILI9486_DB6_PIN;
    LL_GPIO_Init(GPIOB, &gpio);

    LL_GPIO_SetOutputPin(ILI9486_RES_GPIO, ILI9486_RES_PIN);
    LL_GPIO_SetOutputPin(ILI9486_CS_GPIO, ILI9486_CS_PIN);
    LL_GPIO_SetOutputPin(ILI9486_DCX_GPIO, ILI9486_DCX_PIN);
    LL_GPIO_SetOutputPin(ILI9486_WR_GPIO, ILI9486_WR_PIN);
    LL_GPIO_SetOutputPin(ILI9486_RD_GPIO, ILI9486_RD_PIN);
}

static void set_data_bus_as_output(void)
{
    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull = LL_GPIO_PULL_NO;

    gpio.Pin = ILI9486_DB1_PIN;
    LL_GPIO_Init(GPIOC, &gpio);

    gpio.Pin = ILI9486_DB0_PIN | ILI9486_DB2_PIN | ILI9486_DB7_PIN;
    LL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = ILI9486_DB3_PIN | ILI9486_DB4_PIN |
               ILI9486_DB5_PIN | ILI9486_DB6_PIN;
    LL_GPIO_Init(GPIOB, &gpio);
}

static inline void res_set(void)
{
    ILI9486_RES_GPIO->BSRR = ILI9486_RES_PIN;
}

static inline void res_reset(void)
{
    ILI9486_RES_GPIO->BSRR = (uint32_t)ILI9486_RES_PIN << 16;
}

static inline void cs_set(void)
{
    ILI9486_CS_GPIO->BSRR = ILI9486_CS_PIN;
}

static inline void cs_reset(void)
{
    ILI9486_CS_GPIO->BSRR = (uint32_t)ILI9486_CS_PIN << 16;
}

static inline void dcx_set(void)
{
    ILI9486_DCX_GPIO->BSRR = ILI9486_DCX_PIN;
}

static inline void dcx_reset(void)
{
    ILI9486_DCX_GPIO->BSRR = (uint32_t)ILI9486_DCX_PIN << 16;
}

static inline void wr_set(void)
{
    ILI9486_WR_GPIO->BSRR = ILI9486_WR_PIN;
}

static inline void wr_reset(void)
{
    ILI9486_WR_GPIO->BSRR = (uint32_t)ILI9486_WR_PIN << 16;
}

static inline void bus_cycle_delay(void)
{
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
}

static inline void pulse_wr(void)
{
    wr_reset();
    bus_cycle_delay();
    wr_set();
    bus_cycle_delay();
}

static inline void write_bus(uint8_t data)
{
    const DBLutEntry *entry = &g_dbLut[data];
    GPIOA->BSRR = entry->bsrrA;
    GPIOB->BSRR = entry->bsrrB;
    GPIOC->BSRR = entry->bsrrC;
}

static void bus_set_reset(void *context, bool asserted)
{
    (void)context;
    if (asserted) {
        res_reset();
    } else {
        res_set();
    }
}

static void bus_write_command(void *context, uint8_t command)
{
    (void)context;
    cs_reset();
    dcx_reset();
    write_bus(command);
    pulse_wr();
    cs_set();
}

static void bus_write_data(void *context, uint8_t data)
{
    (void)context;
    cs_reset();
    dcx_set();
    write_bus(data);
    pulse_wr();
    cs_set();
}

static void bus_begin_pixels(void *context)
{
    (void)context;
    cs_reset();
    dcx_set();
}

static void bus_end_pixels(void *context)
{
    (void)context;
    cs_set();
}

static void bus_push_color(void *context, uint16_t color, uint32_t count)
{
    (void)context;
    uint8_t high = (uint8_t)(color >> 8);
    uint8_t low = (uint8_t)(color & 0xFFU);

    while (count-- > 0U) {
        write_bus(high);
        pulse_wr();
        write_bus(low);
        pulse_wr();
    }
}

static void bus_push_pixels(void *context,
                            const uint16_t *pixels,
                            uint32_t count)
{
    (void)context;
    while (count-- > 0U) {
        uint16_t color = *pixels++;
        write_bus((uint8_t)(color >> 8));
        pulse_wr();
        write_bus((uint8_t)(color & 0xFFU));
        pulse_wr();
    }
}

static void bus_delay_ms(void *context, uint32_t milliseconds)
{
    (void)context;
    LL_mDelay(milliseconds);
}

bool platform_nucleof411re_ili9486_init(void)
{
    if (s_ili9486.initialized) {
        return true;
    }

    init_interface_pins();
    set_data_bus_as_output();

    const ili9486_bus_t bus = {
        .context = NULL,
        .set_reset = bus_set_reset,
        .write_command = bus_write_command,
        .write_data = bus_write_data,
        .begin_pixels = bus_begin_pixels,
        .end_pixels = bus_end_pixels,
        .push_color = bus_push_color,
        .push_pixels_rgb565 = bus_push_pixels,
        .delay_ms = bus_delay_ms,
    };
    const ili9486_config_t config = {
        .width = ILI9486_WIDTH,
        .height = ILI9486_HEIGHT,
        .mirror_x = true,
        .mirror_y = false,
        .swap_xy = false,
        .bgr = true,
    };

    return ili9486_init(&s_ili9486, &bus, &config);
}

const display_hal_t *platform_nucleof411re_ili9486_display(void)
{
    return ili9486_display(&s_ili9486);
}
