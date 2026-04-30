#include "platform_nucleof411re_buttons.h"
#include "platform/nucleof411re/systick.h"

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
static bool                                 s_raw_prev[BTN_COUNT];
static uint32_t                             s_raw_last_change_ms[BTN_COUNT];
static bool                                 s_initialized = false;

#define BUTTON_DEBOUNCE_MS 25u

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
    memset(s_raw_prev, 0, sizeof(s_raw_prev));
    memset(s_raw_last_change_ms, 0, sizeof(s_raw_last_change_ms));

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
    uint32_t now_ms;

    if (!s_initialized) {
        return;
    }

    now_ms = systick_millis();

    for (int i = 0; i < BTN_COUNT; i++) {
        bool raw_current = prv_read_button((button_id_t)i);

        /* Detecta cambio en la señal cruda para abrir nueva ventana debounce. */
        if (raw_current != s_raw_prev[i]) {
            s_raw_prev[i] = raw_current;
            s_raw_last_change_ms[i] = now_ms;
        }

        s_state.pressed [i] = false;
        s_state.released[i] = false;

        /* Solo acepta el nuevo estado si permanece estable >= debounce ms. */
        if ((now_ms - s_raw_last_change_ms[i]) >= BUTTON_DEBOUNCE_MS) {
            if (s_prev[i] != s_raw_prev[i]) {
                bool stable = s_raw_prev[i];
                s_state.pressed [i] = stable && !s_prev[i];
                s_state.released[i] = !stable && s_prev[i];
                s_prev[i] = stable;
            }
        }

        s_state.held[i] = s_prev[i];
    }
}

const button_state_t *platform_nucleof411re_buttons_get(void)
{
    return &s_state;
}
