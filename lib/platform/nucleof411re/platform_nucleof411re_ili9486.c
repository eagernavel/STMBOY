#include "platform_nucleof411re_ili9486.h"
#include "platform/component/ili9486/ili9486.h"

#include <stddef.h>

static void prv_init_ili9486_interface_pins(void)
{
    // TODO init gpios
}

static void prv_res_pin_set(void)
{
    // TODO reset pin set function implementation
}

static void prv_res_pin_reset(void)
{
    // TODO reset pin reset function implementation
}

// TODO implementation of 8 bit parallel interface functions

void platform_nucleof411re_ili9486_init(void)
{
    // TODO set function pointers to function implementations
    const ili9486_8bitParallelInterface interface = {
        .res_pin_set = prv_res_pin_set,
        .res_pin_reset = prv_res_pin_set,
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
