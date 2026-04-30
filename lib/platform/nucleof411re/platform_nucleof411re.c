#include "platform/platform.h"
#include "platform/nucleof411re/platform_nucleof411re_clock.h"
#include "platform/nucleof411re/platform_nucleof411re_serial.h"
#include "platform/nucleof411re/platform_nucleof411re_ili9486.h"
#include "platform/nucleof411re/platform_nucleof411re_buttons.h"

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"

#include <stddef.h>
#include <stdbool.h>

static void prv_init_clock(void)
{
    const platform_nucleof411re_clock_Config clock_config = {
        .pwr_vos_scale = LL_PWR_REGU_VOLTAGE_SCALE1,
        .flash_wait_cycles = LL_FLASH_LATENCY_3,
        .pll = {
            .div_m = LL_RCC_PLLM_DIV_16,
            .mult_n = 400,
            .div_p = LL_RCC_PLLP_DIV_4,
        },
        .ahb_prescaler = LL_RCC_SYSCLK_DIV_1,
        .apb1_prescaler = LL_RCC_APB1_DIV_2,
        .apb2_prescaler = LL_RCC_APB2_DIV_1,
    };
    platform_nucleof411re_clock_init(clock_config);

}

static void prv_init_serial(void)
{
    const platform_nucleof411re_serial_Config serial_config = {
        .tx_pin_config = {
            .gpio_peripheral_enable = LL_AHB1_GRP1_PERIPH_GPIOA,
            .gpio = GPIOA,
            .pin = LL_GPIO_PIN_2,
            .af = LL_GPIO_AF_7,
        },
        .uart_config = {
            .uart_peripheral_enable = LL_APB1_GRP1_PERIPH_USART2,
            .uart = USART2,
            .baudrate = 115200,
        },
    };
    platform_nucleof411re_serial_init(serial_config);
}

static void prv_init_buttons(void)
{
    /*
     * Asignación de pines — fase de diseño:
     *
     *   BTN_UP    → PC0   (libre, clock GPIOC ya habilitado por ILI9486)
     *   BTN_DOWN  → PC2   (libre)
     *   BTN_START → PC3   (libre)
     *
     * Botones conectados entre el pin y GND (active-low).
     * El driver activa el pull-up interno del STM32.
     */
    const platform_nucleof411re_buttons_Config buttons_config = {
        .buttons = {
            [BTN_UP] = {
                .gpio_clk = LL_AHB1_GRP1_PERIPH_GPIOC,
                .gpio     = GPIOC,
                .pin      = LL_GPIO_PIN_0,
            },
            [BTN_DOWN] = {
                .gpio_clk = LL_AHB1_GRP1_PERIPH_GPIOC,
                .gpio     = GPIOC,
                .pin      = LL_GPIO_PIN_2,
            },
            [BTN_START] = {
                .gpio_clk = LL_AHB1_GRP1_PERIPH_GPIOC,
                .gpio     = GPIOC,
                .pin      = LL_GPIO_PIN_3,
            },
        },
    };
    platform_nucleof411re_buttons_init(buttons_config);
}

void platform_init(void)
{
    static bool s_initialized = false;
    if (s_initialized) {
        return;
    }

    prv_init_clock();
    prv_init_serial();
    platform_nucleof411re_ili9486_init();
    prv_init_buttons();
    s_initialized = true;
}
