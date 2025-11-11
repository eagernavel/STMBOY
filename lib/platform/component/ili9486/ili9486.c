#include "ili9486.h"

static ili9486_8bitParallelInterface s_interface;

// now s_interface can be used to perform actions on the pins, e.g. to start a write command:
// s_interface.db70_write(0x2C); // command code 0x2c should be part of an enum, no magical numbers
// s_interface.dcx_pin_reset();
// s_interface.rdx_pin_set();
// s_interface.wrx_pin_reset();
// s_interface.cs_pin_reset(); // assert
// s_interface.wrx_pin_set(); // trigger
// ...

void ili9486_init(ili9486_8bitParallelInterface interface)
{
    s_interface = interface;
}
