#include "platform/platform.h"
#include "platform/nucleof411re/platform_nucleof411re_clock.h"
#include "platform/nucleof411re/platform_nucleof411re_ili9486.h"
#include "platform/nucleof411re/platform_nucleof411re_buttons.h"
#include "platform/nucleof411re/systick.h"

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

static void prv_init_buttons(void)
{
    /*
     * Asignación de pines
     *
     *   INPUT_UP    -> PC0   (clock GPIOC ya habilitado por ILI9486)
     *   INPUT_DOWN  -> PC2
     *   INPUT_START -> PC3
     *
     * Botones conectados entre el pin y GND (active-low).
     * El driver activa el pull-up interno del STM32.
     */
    const platform_nucleof411re_buttons_Config buttons_config = {
        .buttons = {
            [INPUT_UP] = {
                .gpio_clk = LL_AHB1_GRP1_PERIPH_GPIOC,
                .gpio     = GPIOC,
                .pin      = LL_GPIO_PIN_0,
            },
            [INPUT_DOWN] = {
                .gpio_clk = LL_AHB1_GRP1_PERIPH_GPIOC,
                .gpio     = GPIOC,
                .pin      = LL_GPIO_PIN_2,
            },
            [INPUT_START] = {
                .gpio_clk = LL_AHB1_GRP1_PERIPH_GPIOC,
                .gpio     = GPIOC,
                .pin      = LL_GPIO_PIN_3,
            },
        },
    };
    platform_nucleof411re_buttons_init(buttons_config);
}

bool platform_init(void)
{
    static bool s_initialized = false;
    if (s_initialized) {
        return true;
    }

    prv_init_clock();
    if (!platform_nucleof411re_ili9486_init()) {
        return false;
    }
    prv_init_buttons();
    systick_init(SystemCoreClock);
    s_initialized = true;
    return true;
}

const display_hal_t *platform_display(void)
{
    return platform_nucleof411re_ili9486_display();
}

void platform_input_update(void)
{
    platform_nucleof411re_buttons_update();
}

const input_state_t *platform_input_get(void)
{
    return platform_nucleof411re_buttons_get();
}

uint32_t platform_millis(void)
{
    return systick_millis();
}
