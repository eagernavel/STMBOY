
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32_assert.h"

#include "cmsis_compiler.h"

/* STM32 LL Assert */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
    printf("[ASSERT] LL assert failed: %s, line %lu\r\n",
           (const char *)file, (unsigned long)line);

    /* Disable all IRQs */
    __disable_irq();
    
    /* Blocking loop */
    for(;;);
}
#endif

/* libc assert */
void __assert_func (const char *file, int line, const char *function, const char *expression) {
    printf("[ASSERT] %s:%d  func=%s  expr=(%s)\r\n",
           file, line, function ? function : "?", expression ? expression : "?");

    /* Disable all IRQs */
    __disable_irq();
    
    /* Blocking loop */
    for(;;);
}


