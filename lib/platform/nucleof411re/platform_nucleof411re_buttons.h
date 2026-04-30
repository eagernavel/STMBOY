#ifndef PLATFORM_NUCLEOF411RE_BUTTONS_H
#define PLATFORM_NUCLEOF411RE_BUTTONS_H

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_ll_gpio.h"

/* -----------------------------------------------------------------------
 * Identificadores de botón.
 *
 * Extensible: añadir nuevos botones aquí y en la Config de init.
 * El orden del enum NO debe cambiar una vez el firmware está en uso
 * (rompe compatibilidad binaria con datos persistidos).
 * ----------------------------------------------------------------------- */
typedef enum {
    BTN_UP    = 0,
    BTN_DOWN  = 1,
    BTN_START = 2,
    BTN_COUNT           /* Siempre al final — no usar como botón */
} button_id_t;

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
    platform_nucleof411re_button_PinConfig buttons[BTN_COUNT];
} platform_nucleof411re_buttons_Config;

/* -----------------------------------------------------------------------
 * Estado de los botones.
 *
 * held     → el botón está pulsado en este instante.
 * pressed  → flanco de bajada: pulsado ESTE frame, libre el anterior.
 * released → flanco de subida: libre ESTE frame, pulsado el anterior.
 *
 * Los flancos son especialmente útiles para menús (una pulsación = una
 * acción) y evitan el efecto de repetición infinita al mantener pulsado.
 * ----------------------------------------------------------------------- */
typedef struct {
    bool held    [BTN_COUNT];
    bool pressed [BTN_COUNT];   /* Flanco bajada — evento puntual */
    bool released[BTN_COUNT];   /* Flanco subida  — evento puntual */
} button_state_t;

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
 * Debe llamarse UNA VEZ por frame, antes de consultar button_state_get().
 * No es reentrante.
 */
void platform_nucleof411re_buttons_update(void);

/**
 * Devuelve un puntero al estado actual de los botones.
 *
 * Válido hasta la próxima llamada a platform_nucleof411re_buttons_update().
 */
const button_state_t *platform_nucleof411re_buttons_get(void);

#endif /* PLATFORM_NUCLEOF411RE_BUTTONS_H */
