#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/*
 * Inicializa SysTick para generar una interrupcion cada 1 ms.
 * core_clock_hz = frecuencia actual del core en Hz
 * Ejemplo: 16000000U o 100000000U
 */
void systick_init(uint32_t core_clock_hz);

/*
 * Devuelve el contador de milisegundos desde que se inicializo SysTick.
 */
uint32_t systick_millis(void);

/*
 * Delay bloqueante sencillo.
 * Util para pruebas, aunque para animaciones es mejor usar systick_millis().
 */
void systick_delay_ms(uint32_t ms);

#endif 
