/*
 * Prueba manual de DB0..DB7 usada durante la validacion del montaje.
 *
 * Para ejecutarla se sustituyo temporalmente source/main.c por este codigo.
 * Debe usarse sin el shield y con una resistencia de 1 kohm por LED.
 */

#include <stddef.h>
#include <stdint.h>

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_utils.h"

#define START_PAUSE_MS       3000U
#define SINGLE_LINE_TIME_MS  3000U
#define PATTERN_HOLD_TIME_MS 5000U
#define GAP_TIME_MS          1000U

typedef struct {
    GPIO_TypeDef *gpio;
    uint32_t pin;
} bus_pin_t;

static const bus_pin_t s_bus_pins[] = {
    {GPIOA, LL_GPIO_PIN_9},  /* DB0 / D8 */
    {GPIOC, LL_GPIO_PIN_7},  /* DB1 / D9 */
    {GPIOA, LL_GPIO_PIN_10}, /* DB2 / D2 */
    {GPIOB, LL_GPIO_PIN_3},  /* DB3 / D3 */
    {GPIOB, LL_GPIO_PIN_5},  /* DB4 / D4 */
    {GPIOB, LL_GPIO_PIN_4},  /* DB5 / D5 */
    {GPIOB, LL_GPIO_PIN_10}, /* DB6 / D6 */
    {GPIOA, LL_GPIO_PIN_8},  /* DB7 / D7 */
};

static const uint8_t s_patterns[] = {
    0x00U,
    0xFFU,
    0x55U,
    0xAAU,
    0x0FU,
    0xF0U,
    0x81U,
    0x7EU,
};

static void init_bus(void)
{
    SystemCoreClockUpdate();
    LL_Init1msTick(SystemCoreClock);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

    for (size_t i = 0U; i < (sizeof(s_bus_pins) / sizeof(s_bus_pins[0])); ++i) {
        GPIO_TypeDef *gpio = s_bus_pins[i].gpio;
        const uint32_t pin = s_bus_pins[i].pin;

        LL_GPIO_ResetOutputPin(gpio, pin);
        LL_GPIO_SetPinOutputType(gpio, pin, LL_GPIO_OUTPUT_PUSHPULL);
        LL_GPIO_SetPinSpeed(gpio, pin, LL_GPIO_SPEED_FREQ_LOW);
        LL_GPIO_SetPinPull(gpio, pin, LL_GPIO_PULL_NO);
        LL_GPIO_SetPinMode(gpio, pin, LL_GPIO_MODE_OUTPUT);
    }
}

static void write_bus(uint8_t value)
{
    for (size_t bit = 0U; bit < (sizeof(s_bus_pins) / sizeof(s_bus_pins[0])); ++bit) {
        const bus_pin_t *bus_pin = &s_bus_pins[bit];

        if ((value & (uint8_t)(1U << bit)) != 0U) {
            LL_GPIO_SetOutputPin(bus_pin->gpio, bus_pin->pin);
        } else {
            LL_GPIO_ResetOutputPin(bus_pin->gpio, bus_pin->pin);
        }
    }
}

int main(void)
{
    init_bus();

    while (1) {
        write_bus(0x00U);
        LL_mDelay(START_PAUSE_MS);

        /* Primera fase: una unica linea activa en cada paso. */
        for (size_t bit = 0U; bit < (sizeof(s_bus_pins) / sizeof(s_bus_pins[0])); ++bit) {
            write_bus((uint8_t)(1U << bit));
            LL_mDelay(SINGLE_LINE_TIME_MS);
            write_bus(0x00U);
            LL_mDelay(GAP_TIME_MS);
        }

        /* Segunda fase: patrones combinados de la memoria. */
        for (size_t i = 0U; i < (sizeof(s_patterns) / sizeof(s_patterns[0])); ++i) {
            write_bus(s_patterns[i]);
            LL_mDelay(PATTERN_HOLD_TIME_MS);
            write_bus(0x00U);
            LL_mDelay(GAP_TIME_MS);
        }
    }
}

