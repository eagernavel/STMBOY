
#include <assert.h>
#include <stdint.h>
#include "stm32_assert.h"

#include "cmsis_compiler.h"

/* STM32 LL Assert */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
    /* Provide empty implementation for now */
    (void)file;
    (void)line;

    /* Disable all IRQs */
    __disable_irq();
    
    /* Blocking loop */
    for(;;);
}
#endif

/* libc assert */
void __assert_func (const char *file, int line, const char *function, const char *expression) {
    (void)file;
    (void)line;
    (void)function;
    (void)expression;

    /* Disable all IRQs */
    __disable_irq();
    
    /* Blocking loop */
    for(;;);
}


