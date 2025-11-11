#ifndef PLATFORM_NUCLEOF411RE_SERIAL_H
#define PLATFORM_NUCLEOF411RE_SERIAL_H

#include <stdint.h>

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"

typedef struct {
    /** Macro of type LL_AHB1_GRP1_PERIPH_GPIOx */
    uint32_t gpio_peripheral_enable;
    /** Macro of type GPIOx */
    GPIO_TypeDef *gpio;
    /** Macro of type LL_GPIO_PIN_xx */
    uint32_t pin;
    /** Macro of type LL_GPIO_AFxx */
    uint32_t af;
} platform_nucleof411re_serial_PinConfig;

typedef struct {
    /** Macro of type LL_APBx_GRP1_PERIPH_USARTx */
    uint32_t uart_peripheral_enable;
    /** Macro of type USARTx */
    USART_TypeDef *uart;
    uint32_t baudrate;
} platform_nucleof411re_serial_UartConfig;

typedef struct {
    platform_nucleof411re_serial_PinConfig tx_pin_config;
    platform_nucleof411re_serial_UartConfig uart_config;
} platform_nucleof411re_serial_Config;

void platform_nucleof411re_serial_init(const platform_nucleof411re_serial_Config config);

#endif