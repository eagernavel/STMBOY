#include "platform_nucleof411re_clock.h"

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"

void platform_nucleof411re_clock_init(const platform_nucleof411re_clock_Config config)
{
    /* Enable HSI RC clock */
    LL_RCC_HSI_Enable();
    while(!LL_RCC_HSI_IsReady());

    /* Power voltage regulator scaling 3 to reach max frequency of 100 MHz */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_PWR_SetRegulVoltageScaling(config.pwr_vos_scale);

    /* Configure FLASH wait cycles */
    LL_FLASH_SetLatency(config.flash_wait_cycles);
    while(LL_FLASH_GetLatency() != config.flash_wait_cycles);

    /* Configure PLL for max frequency of 100MHz */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, config.pll.div_m, config.pll.mult_n, config.pll.div_p);

    /* Enable PLL */
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady());

    /* Make sure voltage regulator is ready */
    while(!LL_PWR_IsActiveFlag_VOS());

    /* Configure pre-scalers */
    LL_RCC_SetAHBPrescaler(config.ahb_prescaler);

    /* APB1 Max Frequency is 50MHz */
    LL_RCC_SetAPB1Prescaler(config.apb1_prescaler);
    LL_RCC_SetAPB2Prescaler(config.apb2_prescaler);

    /* Select PLL as clock source */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    /* Update SystemCoreClock */
    SystemCoreClockUpdate();
}