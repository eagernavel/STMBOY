#ifndef PLATFORM_NUCLEOF411RE_BUTTONS_H
#define PLATFORM_NUCLEOF411RE_BUTTONS_H

#include <stdint.h>

#include "common/input.h"
#include "stm32f4xx_ll_gpio.h"

/* -----------------------------------------------------------------------
 * Configuración de un pin de botón.
 *
 * Misma filosofía que platform_nucleof411re_serial_PinConfig:
 * datos puros, sin lógica, inyectados desde platform_nucleof411re.c.
 * ----------------------------------------------------------------------- */
typedef struct {
    /** Macro tipo LL_AHB1_GRP1_PERIPH_GPIOx — para habilitar el clock */
    uint32_t     gpio_clk;
    /** Puerto GPIO (GPIOA, GPIOB, GPIOC…) */
    GPIO_TypeDef *gpio;
    /** Máscara de pin (LL_GPIO_PIN_x) */
    uint32_t     pin;
} platform_nucleof411re_button_PinConfig;

typedef struct {
    platform_nucleof411re_button_PinConfig buttons[INPUT_BUTTON_COUNT];
} platform_nucleof411re_buttons_Config;

/* -----------------------------------------------------------------------
 * API pública
 * ----------------------------------------------------------------------- */

/**
 * Inicializa los pines de botón como entradas con pull-up interno.
 * Los botones deben conectarse entre el pin y GND (active-low).
 *
 * Llamar UNA VEZ durante platform_init().
 */
void platform_nucleof411re_buttons_init(
    const platform_nucleof411re_buttons_Config config);

/**
 * Lee el estado físico de todos los botones y actualiza los flancos.
 *
 * La señal se filtra con debounce temporal interno (ventana fija en ms),
 * por lo que pressed/released solo se emiten cuando el estado permanece
 * estable el tiempo mínimo configurado en la implementación.
 *
 * Debe llamarse UNA VEZ por frame, antes de consultar button_state_get().
 * No es reentrante.
 */
void platform_nucleof411re_buttons_update(void);

/**
 * Devuelve un puntero al estado actual de los botones.
 *
 * Válido hasta la próxima llamada a platform_nucleof411re_buttons_update().
 */
const input_state_t *platform_nucleof411re_buttons_get(void);

#endif /* PLATFORM_NUCLEOF411RE_BUTTONS_H */
