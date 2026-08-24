#include <stdint.h>
#include <stdio.h>

#include "cmsis_compiler.h"
#include "stm32_assert.h"

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    printf("[ASSERT] LL assert failed: %s, line %lu\r\n",
           (const char *)file, (unsigned long)line);
    __disable_irq();
    for (;;) {
    }
}
#endif

void __assert_func(const char *file,
                   int line,
                   const char *function,
                   const char *expression)
{
    printf("[ASSERT] %s:%d func=%s expr=(%s)\r\n",
           file, line,
           function ? function : "?",
           expression ? expression : "?");
    __disable_irq();
    for (;;) {
    }
}
