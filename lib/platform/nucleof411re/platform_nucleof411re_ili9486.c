#include "platform_nucleof411re_ili9486.h"
#include "platform/component/ili9486/ili9486.h"

#include "stm32f411xe.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

#include <stddef.h>
#include <stdint.h>

#define ILI9486_RES_GPIO GPIOC
#define ILI9486_RES_PIN  LL_GPIO_PIN_1 //PC1

#define ILI9486_CS_GPIO  GPIOB
#define ILI9486_CS_PIN   LL_GPIO_PIN_0 //PB0

#define ILI9486_DCX_GPIO  GPIOA
#define ILI9486_DCX_PIN  LL_GPIO_PIN_4 //PA4

#define ILI9486_WR_GPIO  GPIOA
#define ILI9486_WR_PIN  LL_GPIO_PIN_1 //PA1

#define ILI9486_RD_GPIO  GPIOA
#define ILI9486_RD_PIN  LL_GPIO_PIN_0 //PA0




static void prv_init_ili9486_interface_pins(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    
    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Pin = ILI9486_RES_PIN;
    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull = LL_GPIO_PULL_NO;

    LL_GPIO_Init(ILI9486_RES_GPIO, &gpio);
    LL_GPIO_SetOutputPin(ILI9486_RES_GPIO, ILI9486_RES_PIN); //RES = 1
    LL_GPIO_SetOutputPin(ILI9486_CS_GPIO,  ILI9486_CS_PIN); // CS = 1 --> ACTIVO ESTA EN MODO BAJO
    LL_GPIO_SetOutputPin(ILI9486_DCX_GPIO, ILI9486_DCX_PIN);// DCX = 1 --> MODO DATA
}

static void prv_res_pin_set(void)
{
    LL_GPIO_SetOutputPin(ILI9486_RES_GPIO, ILI9486_RES_PIN);// TODO reset pin set function implementation
}

static void prv_res_pin_reset(void)
{
    LL_GPIO_ResetOutputPin(ILI9486_RES_GPIO, ILI9486_RES_PIN);// TODO reset pin reset function implementation
}

static void prv_cs_pin_set(void) //seleccionar o no la pantalla
{
    LL_GPIO_SetOutputPin(ILI9486_CS_GPIO, ILI9486_CS_PIN); //INACTIVO
}

static void prv_cs_pin_reset(void)
{
    LL_GPIO_ResetOutputPin(ILI9486_CS_GPIO, ILI9486_CS_PIN); //ACTIVO
}

static void prv_dcx_pin_set(void)
{
    LL_GPIO_SetOutputPin(ILI9486_DCX_GPIO, ILI9486_DCX_PIN); //le dice que tipo de dato es, COMMAND o DATA
}

static void prv_dcx_pin_reset(void)
{
    LL_GPIO_ResetOutputPin(ILI9486_DCX_GPIO, ILI9486_DCX_PIN);
}

static void prv_wr_pin_set(void)
{
    LL_GPIO_SetOutputPin(ILI9486_WR_GPIO, ILI9486_WR_PIN); //le dice que tipo de dato es, COMMAND o DATA
}

static void prv_wr_pin_reset(void)
{
    LL_GPIO_ResetOutputPin(ILI9486_WR_GPIO, ILI9486_WR_PIN);
}

static void prv_rd_pin_set(void)
{
    LL_GPIO_SetOutputPin(ILI9486_DCX_GPIO, ILI9486_DCX_PIN); //le dice que tipo de dato es, COMMAND o DATA
}

static void prv_rd_pin_reset(void)
{
    LL_GPIO_ResetOutputPin(ILI9486_DCX_GPIO, ILI9486_DCX_PIN);
}


// TODO implementation of 8 bit parallel interface functions

void platform_nucleof411re_ili9486_init(void)
{
    
    // TODO set function pointers to function implementations
    prv_init_ili9486_interface_pins();
    const ili9486_8bitParallelInterface interface = {
        .res_pin_set = prv_res_pin_set,
        .res_pin_reset = prv_res_pin_reset,
        .cs_pin_set = prv_cs_pin_set,
        .cs_pin_reset = prv_cs_pin_reset,
        .dcx_pin_set = prv_dcx_pin_set,
        .dcx_pin_reset = prv_dcx_pin_reset,
        .wrx_pin_set = prv_wr_pin_set,
        .wrx_pin_reset = prv_cs_pin_reset,
        .rdx_pin_set = prv_cs_pin_set,
        .rdx_pin_reset = prv_cs_pin_reset,
        .db70_write = NULL,
        .db70_read = NULL,
    };
    ili9486_init(interface);
}
