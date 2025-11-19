#include "platform_nucleof411re_ili9486.h"
#include "platform/component/ili9486/ili9486.h"

#include "stm32f411xe.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

#include <stddef.h>
#include <stdint.h>

#define ILI9486_RES_GPIO GPIOC
#define ILI9486_RES_PIN  LL_GPIO_PIN_0

static void prv_init_ili9486_interface_pins(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    
    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Pin = ILI9486_RES_PIN;
    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull = LL_GPIO_PULL_NO;

    LL_GPIO_Init(ILI9486_RES_GPIO, &gpio);
    LL_GPIO_SetOutputPin(ILI9486_RES_GPIO, ILI9486_RES_PIN);// TODO init gpios
}

static void prv_res_pin_set(void)
{
    LL_GPIO_SetOutputPin(ILI9486_RES_GPIO, ILI9486_RES_PIN);// TODO reset pin set function implementation
}

static void prv_res_pin_reset(void)
{
    LL_GPIO_SetOutputPin(ILI9486_RES_GPIO, ILI9486_RES_PIN);// TODO reset pin reset function implementation
}

// TODO implementation of 8 bit parallel interface functions

void platform_nucleof411re_ili9486_init(void)
{
    // TODO set function pointers to function implementations
    const ili9486_8bitParallelInterface interface = {
        .res_pin_set = prv_res_pin_set,
        .res_pin_reset = prv_res_pin_reset,
        .cs_pin_set = NULL,
        .cs_pin_reset = NULL,
        .dcx_pin_set = NULL,
        .dcx_pin_reset = NULL,
        .wrx_pin_set = NULL,
        .wrx_pin_reset = NULL,
        .rdx_pin_set = NULL,
        .rdx_pin_reset = NULL,
        .db70_write = NULL,
        .db70_read = NULL,
    };
    ili9486_init(interface);
}
