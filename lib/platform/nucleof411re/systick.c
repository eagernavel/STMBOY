#include "systick.h"
#include "stm32f411xe.h"

static volatile uint32_t g_systick_ms = 0;

void systick_init(uint32_t core_clock_hz)
{
    uint32_t reload;

    if (core_clock_hz == 0U) {
        return;
    }

    /*
     * Queremos 1 tick cada 1 ms:
     * reload = (f_core / 1000) - 1
     */
    reload = (core_clock_hz / 1000U) - 1U;

    /*
     * SysTick es de 24 bits, asi que LOAD no puede superar 0xFFFFFF.
     */
    if (reload > 0x00FFFFFFU) {
        return;
    }

    g_systick_ms = 0;

    SysTick->LOAD = reload;
    SysTick->VAL  = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |  /* reloj del core */
                    SysTick_CTRL_TICKINT_Msk   |  /* habilita interrupcion */
                    SysTick_CTRL_ENABLE_Msk;      /* arranca SysTick */
}

uint32_t systick_millis(void)
{
    return g_systick_ms;
}

void systick_delay_ms(uint32_t ms)
{
    uint32_t start = systick_millis();

    while ((systick_millis() - start) < ms) {
        /* espera bloqueante */
    }
}

void SysTick_Handler(void)
{
    g_systick_ms++;
}