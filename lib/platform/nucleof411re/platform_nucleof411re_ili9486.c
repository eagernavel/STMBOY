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

#define ILI9486_DB0_GPIO GPIOA
#define ILI9486_DB0_PIN  LL_GPIO_PIN_9  //PA9
#define ILI9486_DB1_GPIO GPIOC
#define ILI9486_DB1_PIN  LL_GPIO_PIN_7  //PC7
#define ILI9486_DB2_GPIO GPIOA
#define ILI9486_DB2_PIN  LL_GPIO_PIN_10  //PA10
#define ILI9486_DB3_GPIO GPIOB
#define ILI9486_DB3_PIN  LL_GPIO_PIN_3 //PB3
#define ILI9486_DB4_GPIO GPIOB
#define ILI9486_DB4_PIN  LL_GPIO_PIN_5 //PB5
#define ILI9486_DB5_GPIO GPIOB
#define ILI9486_DB5_PIN  LL_GPIO_PIN_4 //PB4
#define ILI9486_DB6_GPIO GPIOB
#define ILI9486_DB6_PIN  LL_GPIO_PIN_10 //PA10
#define ILI9486_DB7_GPIO GPIOA
#define ILI9486_DB7_PIN  LL_GPIO_PIN_8 //PA8




static void prv_init_ili9486_interface_pins(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    
    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull = LL_GPIO_PULL_NO;

    gpio.Pin = ILI9486_RES_PIN | ILI9486_DB1_PIN;
    LL_GPIO_Init(GPIOC, &gpio);  //GPIOC

    gpio.Pin = ILI9486_WR_PIN | ILI9486_RD_PIN | ILI9486_DB0_PIN | ILI9486_DB2_PIN | ILI9486_DB7_PIN;
    LL_GPIO_Init(GPIOA, &gpio);  //GPIOA

    gpio.Pin = ILI9486_CS_PIN | ILI9486_DB3_PIN | ILI9486_DB4_PIN | ILI9486_DB5_PIN | ILI9486_DB6_PIN;
    LL_GPIO_Init(GPIOB, &gpio);  //GPIOB
    

    // ESta parte es para definir un estado en concreto al inicio

    LL_GPIO_SetOutputPin(ILI9486_RES_GPIO, ILI9486_RES_PIN); //RES = 1
    LL_GPIO_SetOutputPin(ILI9486_CS_GPIO,  ILI9486_CS_PIN); // CS = 1 --> ACTIVO ESTA EN MODO BAJO
    LL_GPIO_SetOutputPin(ILI9486_DCX_GPIO, ILI9486_DCX_PIN); // DCX = 1 --> MODO DATA
    LL_GPIO_SetOutputPin(ILI9486_WR_GPIO,  ILI9486_WR_PIN);  // WR = 1 (inactivo)
    LL_GPIO_SetOutputPin(ILI9486_RD_GPIO,  ILI9486_RD_PIN);
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
    LL_GPIO_SetOutputPin(ILI9486_WR_GPIO, ILI9486_WR_PIN); //
}

static void prv_wr_pin_reset(void)
{
    LL_GPIO_ResetOutputPin(ILI9486_WR_GPIO, ILI9486_WR_PIN);
}

static void prv_rd_pin_set(void)
{
    LL_GPIO_SetOutputPin(ILI9486_RD_GPIO, ILI9486_RD_PIN); //
}

static void prv_rd_pin_reset(void)
{
    LL_GPIO_ResetOutputPin(ILI9486_RD_GPIO, ILI9486_RD_PIN);
}

static void prv_db70_write(uint8_t data)
{
    if (data & (1 << 0))
        LL_GPIO_SetOutputPin(ILI9486_DB0_GPIO, ILI9486_DB0_PIN);
    else
        LL_GPIO_ResetOutputPin(ILI9486_DB0_GPIO, ILI9486_DB0_PIN);

    if (data & (1 << 1))
        LL_GPIO_SetOutputPin(ILI9486_DB1_GPIO, ILI9486_DB1_PIN);
    else
        LL_GPIO_ResetOutputPin(ILI9486_DB1_GPIO, ILI9486_DB1_PIN);

    if (data & (1 << 2))
        LL_GPIO_SetOutputPin(ILI9486_DB2_GPIO, ILI9486_DB2_PIN);
    else
        LL_GPIO_ResetOutputPin(ILI9486_DB2_GPIO, ILI9486_DB2_PIN);

    if (data & (1 << 3))
        LL_GPIO_SetOutputPin(ILI9486_DB3_GPIO, ILI9486_DB3_PIN);
    else
        LL_GPIO_ResetOutputPin(ILI9486_DB3_GPIO, ILI9486_DB3_PIN);

    if (data & (1 << 4))
        LL_GPIO_SetOutputPin(ILI9486_DB4_GPIO, ILI9486_DB4_PIN);
    else
        LL_GPIO_ResetOutputPin(ILI9486_DB4_GPIO, ILI9486_DB4_PIN);

    if (data & (1 << 5))
        LL_GPIO_SetOutputPin(ILI9486_DB5_GPIO, ILI9486_DB5_PIN);
    else
        LL_GPIO_ResetOutputPin(ILI9486_DB5_GPIO, ILI9486_DB5_PIN);

    if (data & (1 << 6))
        LL_GPIO_SetOutputPin(ILI9486_DB6_GPIO, ILI9486_DB6_PIN);
    else
        LL_GPIO_ResetOutputPin(ILI9486_DB6_GPIO, ILI9486_DB6_PIN);
        
    if (data & (1 << 7))
        LL_GPIO_SetOutputPin(ILI9486_DB7_GPIO, ILI9486_DB7_PIN);
    else
        LL_GPIO_ResetOutputPin(ILI9486_DB7_GPIO, ILI9486_DB7_PIN);  
        // TODO implementation of DB[7:0] write function
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
