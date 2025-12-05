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
#define ILI9486_DB6_PIN  LL_GPIO_PIN_10 //PB10
#define ILI9486_DB7_GPIO GPIOA
#define ILI9486_DB7_PIN  LL_GPIO_PIN_8 //PA8

#define ILI9486_WIDTH   320
#define ILI9486_HEIGHT  480




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

    gpio.Pin = ILI9486_WR_PIN | ILI9486_RD_PIN | ILI9486_DB0_PIN | ILI9486_DB2_PIN | ILI9486_DB7_PIN | ILI9486_DCX_PIN;
    LL_GPIO_Init(GPIOA, &gpio);  //GPIOA

    gpio.Pin = ILI9486_CS_PIN | ILI9486_DB3_PIN | ILI9486_DB4_PIN | ILI9486_DB5_PIN | ILI9486_DB6_PIN;
    LL_GPIO_Init(GPIOB, &gpio);  //GPIOB
    

    // ESta parte es para definir un estado en concreto al inicio

    LL_GPIO_SetOutputPin(ILI9486_RES_GPIO, ILI9486_RES_PIN); //RES = 1
    LL_GPIO_SetOutputPin(ILI9486_CS_GPIO,  ILI9486_CS_PIN); // CS = 1 --> ACTIVO ESTA EN MODO BAJO
    LL_GPIO_SetOutputPin(ILI9486_DCX_GPIO, ILI9486_DCX_PIN); // DCX = 1 --> MODO DATA
    LL_GPIO_SetOutputPin(ILI9486_WR_GPIO,  ILI9486_WR_PIN);  // WR = 1 (inactivo)
    LL_GPIO_SetOutputPin(ILI9486_RD_GPIO,  ILI9486_RD_PIN); // RD = 1 (inactivo)
}

static void prv_db70_set_as_outputs(void)
{
    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Mode       = LL_GPIO_MODE_OUTPUT;
    gpio.Speed      = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull       = LL_GPIO_PULL_NO;

    gpio.Pin = ILI9486_DB1_PIN;
    LL_GPIO_Init(GPIOC, &gpio);  //GPIOC

    gpio.Pin =ILI9486_DB0_PIN | ILI9486_DB2_PIN | ILI9486_DB7_PIN;
    LL_GPIO_Init(GPIOA, &gpio);  //GPIOA

    gpio.Pin = ILI9486_DB3_PIN | ILI9486_DB4_PIN | ILI9486_DB5_PIN | ILI9486_DB6_PIN;
    LL_GPIO_Init(GPIOB, &gpio);  //GPIOB

}

static void prv_db70_set_as_inputs(void)
{
    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Mode       = LL_GPIO_MODE_INPUT;
    gpio.Speed      = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull       = LL_GPIO_PULL_NO;

    gpio.Pin = ILI9486_DB1_PIN;
    LL_GPIO_Init(GPIOC, &gpio);  //GPIOC

    gpio.Pin =ILI9486_DB0_PIN | ILI9486_DB2_PIN | ILI9486_DB7_PIN;
    LL_GPIO_Init(GPIOA, &gpio);  //GPIOA

    gpio.Pin = ILI9486_DB3_PIN | ILI9486_DB4_PIN | ILI9486_DB5_PIN | ILI9486_DB6_PIN;
    LL_GPIO_Init(GPIOB, &gpio);  //GPIOB

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
      
}

static uint8_t prv_db70_read(void)
{
    uint8_t data = 0;
    prv_db70_set_as_inputs();

    LL_GPIO_ResetOutputPin(ILI9486_RD_GPIO, ILI9486_RD_PIN); // Activar señal de lectura (RD = 0)

    if (LL_GPIO_IsInputPinSet(ILI9486_DB0_GPIO, ILI9486_DB0_PIN))
        data |= (1 << 0);
    if (LL_GPIO_IsInputPinSet(ILI9486_DB1_GPIO, ILI9486_DB1_PIN))
        data |= (1 << 1);
    if (LL_GPIO_IsInputPinSet(ILI9486_DB2_GPIO, ILI9486_DB2_PIN))
        data |= (1 << 2);
    if (LL_GPIO_IsInputPinSet(ILI9486_DB3_GPIO, ILI9486_DB3_PIN))
        data |= (1 << 3);
    if (LL_GPIO_IsInputPinSet(ILI9486_DB4_GPIO, ILI9486_DB4_PIN))
        data |= (1 << 4);
    if (LL_GPIO_IsInputPinSet(ILI9486_DB5_GPIO, ILI9486_DB5_PIN))
        data |= (1 << 5);
    if (LL_GPIO_IsInputPinSet(ILI9486_DB6_GPIO, ILI9486_DB6_PIN))
        data |= (1 << 6);
    if (LL_GPIO_IsInputPinSet(ILI9486_DB7_GPIO, ILI9486_DB7_PIN))
        data |= (1 << 7);

    LL_GPIO_SetOutputPin(ILI9486_RD_GPIO, ILI9486_RD_PIN); // Desactivar señal de lectura (RD = 1)

    prv_db70_set_as_outputs();

    return data;
    // Para conseguir esto necesitamos varias cosas:
    // 1. Configurar los pines de datos como entrada
    // 2. Leer el estado de cada pin y construir el byte de datos
    // 3. Volver a configurar los pines de datos como salida 
}


static void test_db70_write_patterns(void)
{
    const uint8_t patterns[] = {
        0x00, 0xFF, 0x55, 0xAA,
        0x0F, 0xF0, 0x81, 0x7E
    };

    for (unsigned i = 0; i < sizeof(patterns); ++i)
    {
        uint8_t v = patterns[i];

        prv_db70_write(v);

        
        for (volatile uint32_t d = 0; d < 1000000; ++d) { }
    }
}

void platform_nucleof411re_ili9486_selftest(void)
{
    prv_init_ili9486_interface_pins();
    prv_db70_set_as_outputs();
    test_db70_write_patterns();

    prv_db70_write(0xFF); // valor final que queda escrito en los pines
}


//INicializamos la pantalla con el objetivo de rellenarla de rojo//

static void bus_cycle_delay(volatile uint32_t count)
{
    while (count--) {
        __asm__("nop");
    }
}

static void prv_pulse_wr(void)
{
    LL_GPIO_ResetOutputPin(ILI9486_WR_GPIO, ILI9486_WR_PIN);  
    bus_cycle_delay(50);   // WR LOW mínimo
    LL_GPIO_SetOutputPin(ILI9486_WR_GPIO, ILI9486_WR_PIN);
    bus_cycle_delay(50);   // WR HIGH estabilidad
}


static void prv_write_command(uint8_t cmd)
{
    prv_cs_pin_reset();      // seleccionamos pantalla
    prv_dcx_pin_reset();     // COMMAND
    prv_db70_write(cmd);     // pone el comando en D0..D7
    prv_pulse_wr();          // pulso de WR
    prv_cs_pin_set();      // deseleccionamos pantalla
}

static void prv_write_data(uint8_t data)
{
    prv_cs_pin_reset();      
    prv_dcx_pin_set();       
    prv_db70_write(data);    
    prv_pulse_wr();          
    prv_cs_pin_set();        
}

static void prv_ili9486_set_madctl(uint8_t madctl)
{
    prv_write_command(0x36);
    prv_write_data(madctl);
}

static void prv_ili9486_first_init(void)
{
    prv_res_pin_reset();
    bus_cycle_delay(2000000);
    prv_res_pin_set();
    bus_cycle_delay(2000000);

    prv_write_command(0x01);    
    bus_cycle_delay(2000000);

    prv_write_command(0x11);    
    bus_cycle_delay(8000000);   

    //Pixel Format 16 bpp RGB565
    prv_write_command(0x3A);
    prv_write_data(0x55);

    prv_ili9486_set_madctl(0x08);

    //Display ON
    prv_write_command(0x29);
    bus_cycle_delay(2000000);
}



static void prv_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // Column Address Set (0x2A)
    prv_write_command(0x2A);
    prv_write_data(x0 >> 8);
    prv_write_data(x0 & 0xFF);
    prv_write_data(x1 >> 8);
    prv_write_data(x1 & 0xFF);

    // Page Address Set (0x2B)
    prv_write_command(0x2B);
    prv_write_data(y0 >> 8);
    prv_write_data(y0 & 0xFF);
    prv_write_data(y1 >> 8);
    prv_write_data(y1 & 0xFF);
}

static void prv_fill_screen(uint16_t color)
{
    uint32_t total_pixels = (uint32_t)ILI9486_WIDTH * (uint32_t)ILI9486_HEIGHT;
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    prv_set_address_window(0, 0, ILI9486_WIDTH - 1, ILI9486_HEIGHT - 1);

    // Memory Write
    prv_write_command(0x2C);

    prv_cs_pin_reset();
    prv_dcx_pin_set(); // DATA

    for (uint32_t i = 0; i < total_pixels; ++i)
    {
        prv_db70_write(hi);
        prv_pulse_wr();
        prv_db70_write(lo);
        prv_pulse_wr();
    }

    prv_cs_pin_set();
}

void platform_nucleof411re_ili9486_test_fill(uint16_t color)
{
    prv_init_ili9486_interface_pins();
    prv_db70_set_as_outputs();   

    prv_ili9486_first_init();    
    prv_fill_screen(color);      
}

void platform_nucleof411re_ili9486_test_colors_cycle(void)
{
    prv_init_ili9486_interface_pins();
    prv_db70_set_as_outputs();

    prv_ili9486_first_init();

    while (1)
    {
        // ROJO
        prv_fill_screen(0xF800);
        bus_cycle_delay(8000000);

        // VERDE
        prv_fill_screen(0x07E0);
        bus_cycle_delay(8000000);

        // AZUL
        prv_fill_screen(0x001F);
        bus_cycle_delay(8000000);
    }
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
        .wrx_pin_reset = prv_wr_pin_reset,
        .rdx_pin_set = prv_rd_pin_set,
        .rdx_pin_reset = prv_rd_pin_reset,
        .db70_write = prv_db70_write,
        .db70_read = prv_db70_read,
    };
    ili9486_init(interface);
}
