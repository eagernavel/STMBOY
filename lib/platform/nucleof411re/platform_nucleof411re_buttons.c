#include "platform_nucleof411re_buttons.h"

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

#include <string.h>

/* -----------------------------------------------------------------------
 * Estado interno — módulo singleton.
 *
 * s_config  → pines configurados durante init().
 * s_state   → estado público actual (held + flancos).
 * s_prev    → estado físico del frame anterior (para calcular flancos).
 * ----------------------------------------------------------------------- */
static platform_nucleof411re_buttons_Config s_config;
static button_state_t                       s_state;
static bool                                 s_prev[BTN_COUNT];
static bool                                 s_initialized = false;

/* -----------------------------------------------------------------------
 * Implementación privada
 * ----------------------------------------------------------------------- */

/**
 * Lee el pin físico de un botón.
 *
 * Active-low: botón conectado entre pin y GND, pull-up interno activado.
 * Pin a nivel bajo  (0) → botón pulsado   → devuelve true.
 * Pin a nivel alto  (1) → botón libre     → devuelve false.
 */
static inline bool prv_read_button(button_id_t id)
{
    return !LL_GPIO_IsInputPinSet(s_config.buttons[id].gpio,
                                  s_config.buttons[id].pin);
}

/* -----------------------------------------------------------------------
 * API pública
 * ----------------------------------------------------------------------- */

void platform_nucleof411re_buttons_init(
    const platform_nucleof411re_buttons_Config config)
{
    if (s_initialized) {
        return;
    }

    s_config = config;

    /* Inicializar estado a "todo libre" */
    memset(&s_state, 0, sizeof(s_state));
    memset(s_prev,   0, sizeof(s_prev));

    LL_GPIO_InitTypeDef gpio_cfg = {0};
    gpio_cfg.Mode      = LL_GPIO_MODE_INPUT;
    gpio_cfg.Pull      = LL_GPIO_PULL_UP;   /* Pull-up interno — botón a GND */
    gpio_cfg.Speed     = LL_GPIO_SPEED_FREQ_LOW;

    for (int i = 0; i < BTN_COUNT; i++) {
        /* Habilitar clock del puerto si no lo está ya */
        LL_AHB1_GRP1_EnableClock(config.buttons[i].gpio_clk);

        gpio_cfg.Pin = config.buttons[i].pin;
        LL_GPIO_Init(config.buttons[i].gpio, &gpio_cfg);
    }

    s_initialized = true;
}

void platform_nucleof411re_buttons_update(void)
{
    if (!s_initialized) {
        return;
    }

    for (int i = 0; i < BTN_COUNT; i++) {
        bool current = prv_read_button((button_id_t)i);

        /* Flancos: comparar con estado del frame anterior */
        s_state.pressed [i] = current && !s_prev[i];   /* 0→1: flanco bajada */
        s_state.released[i] = !current && s_prev[i];   /* 1→0: flanco subida */
        s_state.held    [i] = current;

        s_prev[i] = current;
    }
}

const button_state_t *platform_nucleof411re_buttons_get(void)
{
    return &s_state;
}
