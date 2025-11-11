#ifndef PLATFORM_COMPONENT_ILI9486_H
#define PLATFORM_COMPONENT_ILI9486_H

#include <stdint.h>

typedef struct {
    // Function interfaces to control External Reset Pin
    void (*res_pin_set)(void);
    void (*res_pin_reset)(void);
    // Function interfaces to control Chip Select Pin
    void (*cs_pin_set)(void);
    void (*cs_pin_reset)(void);
    // Function interfaces to control Data/Command Pin
    void (*dcx_pin_set)(void);
    void (*dcx_pin_reset)(void);
    // Function interfaces to control Write Singal Pin
    void (*wrx_pin_set)(void);
    void (*wrx_pin_reset)(void);
    // Function interfaces to control Read Singal Pin
    void (*rdx_pin_set)(void);
    void (*rdx_pin_reset)(void);
    // Function interface to write DB[7:0]
    void (*db70_write)(uint8_t data);
    // Function interface to read DB[7:0]
    uint8_t (*db70_read)(void);
} ili9486_8bitParallelInterface;

void ili9486_init(ili9486_8bitParallelInterface interface);

#endif
