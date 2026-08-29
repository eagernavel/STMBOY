#include <stdint.h>

#include "cmsis_compiler.h"
#include "stm32_assert.h"

static void halt_on_assert(void)
{
    __disable_irq();
    for (;;) {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
    halt_on_assert();
}
#endif

void __assert_func(const char *file,
                   int line,
                   const char *function,
                   const char *expression)
{
    (void)file;
    (void)line;
    (void)function;
    (void)expression;
    halt_on_assert();
}
