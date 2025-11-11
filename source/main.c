#include <stdint.h>
#include <stdio.h>

#include "bsp/bsp.h"

static inline void raw_delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
  }

int main(void) {

    bsp_init();

    while(1) {
        printf("Hello World!\n\r");
        raw_delay(4000000);
    }

    return 0;
}
